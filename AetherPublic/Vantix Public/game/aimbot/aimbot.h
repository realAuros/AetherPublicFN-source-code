#pragma once
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <Psapi.h>
#include "../../includes.h"
#include <thread>
#include <cmath>
#include <chrono>
#include <random>
#include <algorithm>
#include "aimbot.hpp"
#include "../memory/driver.h"
#include "../aimbot/callbackspoofer.h"

struct csvector2 { double x, y; };

extern CallbackSpoofer g_CallbackSpoofer;

struct ENCRYPTED_OFFSETS {
    uintptr_t net_connection;
    uintptr_t rotation_input;
    uint64_t encryption_key;
    bool encrypted;

    ENCRYPTED_OFFSETS() : net_connection(0x520), rotation_input(0x528),
        encryption_key(0xDEADBEEFCAFEBABE), encrypted(true) {
    }

    uintptr_t GetNetConnection() const {
        return encrypted ? (net_connection ^ encryption_key) : net_connection;
    }

    uintptr_t GetRotationInput() const {
        return encrypted ? (rotation_input ^ encryption_key) : rotation_input;
    }
};

extern ENCRYPTED_OFFSETS g_EncryptedOffsets;

class AimbotProtection {
private:
    static bool s_initialized;

public:
    static bool Initialize() {
        if (s_initialized) return true;

        if (!g_CallbackSpoofer.Initialize()) {
            return false;
        }

        if (!g_CallbackSpoofer.StartSpoofing()) {
            return false;
        }

        s_initialized = true;
        return true;
    }

    static void EnsureProtection() {
        static bool checked = false;
        if (!checked) {
            checked = true;
            Initialize();
        }
    }
};

bool AimbotProtection::s_initialized = false;

bool memory_event(fvector newpos)
{
    AimbotProtection::EnsureProtection();

    uintptr_t rotation_input = g_EncryptedOffsets.GetRotationInput();

    write<fvector>(CachePointers.PlayerController + rotation_input, newpos);
    return true;
}

inline void move(fvector2d Head2D, float AimSpeed)
{
    AimbotProtection::EnsureProtection();

    if (AimSpeed == 0) {
        return;
    }

    const fvector2d ScreenCenter = {
        static_cast<double>(globals.ScreenWidth) / 2,
        static_cast<double>(globals.ScreenHeight) / 2
    };

    fvector2d Target = { 0, 0 };

    if (Head2D.x != 0) {
        double deltaX = (Head2D.x > ScreenCenter.x)
            ? -(ScreenCenter.x - Head2D.x)
            : (Head2D.x - ScreenCenter.x);
        deltaX /= AimSpeed;

        if (deltaX + ScreenCenter.x <= ScreenCenter.x * 2 && deltaX + ScreenCenter.x >= 0) {
            Target.x = deltaX;
        }
    }

    if (Head2D.y != 0) {
        double deltaY = (Head2D.y > ScreenCenter.y)
            ? -(ScreenCenter.y - Head2D.y)
            : (Head2D.y - ScreenCenter.y);
        deltaY /= AimSpeed;

        if (deltaY + ScreenCenter.y <= ScreenCenter.y * 2 && deltaY + ScreenCenter.y >= 0) {
            Target.y = deltaY;
        }
    }

}


inline void memoryMove(fvector2d Head2D, float AimSpeed)
{
    AimbotProtection::EnsureProtection();

    if (AimSpeed == 0) {
        return;
    }

    const fvector2d ScreenCenter = {
        static_cast<double>(globals.ScreenWidth) / 2,
        static_cast<double>(globals.ScreenHeight) / 2
    };

    fvector2d Target = { 0, 0 };

    if (Head2D.x != 0) {
        double deltaX = (Head2D.x > ScreenCenter.x)
            ? -(ScreenCenter.x - Head2D.x)
            : (Head2D.x - ScreenCenter.x);
        deltaX /= AimSpeed;

        if (deltaX + ScreenCenter.x <= ScreenCenter.x * 2 && deltaX + ScreenCenter.x >= 0) {
            Target.x = deltaX;
        }
    }

    if (Head2D.y != 0) {
        double deltaY = (Head2D.y > ScreenCenter.y)
            ? -(ScreenCenter.y - Head2D.y)
            : (Head2D.y - ScreenCenter.y);
        deltaY /= AimSpeed;

        if (deltaY + ScreenCenter.y <= ScreenCenter.y * 2 && deltaY + ScreenCenter.y >= 0) {
            Target.y = deltaY;
        }
    }

    memory_event(fvector(-Target.y / 5, Target.x / 5, 0));
}

namespace humanized_aim {
    inline double overshoot_chance = 0.25;
    inline double overshoot_max_factor = 0.06;
    inline double locked_on_radius = 60.0;
    inline double slowdown_factor = 0.35;
    inline std::chrono::milliseconds slowdown_period{ 2000 };

    inline fvector2d computeMovement(
        const fvector2d& Head2D,
        double AimSpeed,
        int ScreenWidth,
        int ScreenHeight)
    {
        AimbotProtection::EnsureProtection();

        if (AimSpeed <= 0.0) {
            return { 0.0, 0.0 };
        }

        static thread_local std::mt19937 rng((std::random_device())());
        static thread_local std::bernoulli_distribution overshoot_dist(overshoot_chance);
        static thread_local std::uniform_real_distribution<double> overshoot_factor_dist(0.0, overshoot_max_factor);
        static thread_local auto last_toggle = std::chrono::steady_clock::now();
        static thread_local bool slowed = false;

        auto now = std::chrono::steady_clock::now();
        if (now - last_toggle >= slowdown_period) {
            last_toggle = now;
            slowed = !slowed;
        }

        const fvector2d ScreenCenter = {
            static_cast<double>(ScreenWidth) / 2.0,
            static_cast<double>(ScreenHeight) / 2.0
        };

        double rawDeltaX = Head2D.x - ScreenCenter.x;
        double rawDeltaY = Head2D.y - ScreenCenter.y;

        double distance = std::hypot(rawDeltaX, rawDeltaY);

        bool lockedOn = (distance <= locked_on_radius);

        double effectiveAimSpeed = AimSpeed;
        if (lockedOn && slowed) {
            effectiveAimSpeed = (std::max)(0.001, AimSpeed * slowdown_factor);
        }

        double moveX = 0.0;
        double moveY = 0.0;

        if (Head2D.x != 0.0) {
            double deltaX = (rawDeltaX > 0.0) ? rawDeltaX : rawDeltaX;
            moveX = deltaX / effectiveAimSpeed;
        }

        if (Head2D.y != 0.0) {
            double deltaY = (rawDeltaY > 0.0) ? rawDeltaY : rawDeltaY;
            moveY = deltaY / effectiveAimSpeed;
        }

        if (overshoot_dist(rng)) {
            double factor = overshoot_factor_dist(rng);
            moveX += (moveX)*factor;
            moveY += (moveY)*factor;
        }

        {
            std::normal_distribution<double> jitter_dist(0.0, 0.5);
            moveX += jitter_dist(rng);
            moveY += jitter_dist(rng);
        }

        double maxMovePerFrame = (std::max)(ScreenWidth, ScreenHeight) * 0.25;
        if (std::abs(moveX) > maxMovePerFrame) moveX = (moveX > 0 ? 1 : -1) * maxMovePerFrame;
        if (std::abs(moveY) > maxMovePerFrame) moveY = (moveY > 0 ? 1 : -1) * maxMovePerFrame;

        return { moveX, moveY };
    }
}

ENCRYPTED_OFFSETS g_EncryptedOffsets;
