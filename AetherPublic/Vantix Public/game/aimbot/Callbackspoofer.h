#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <random>
#include <chrono>
#include <atomic>
#include <map>

class CallbackSpoofer {
private:
    std::atomic<bool> m_initialized;
    std::atomic<bool> m_spoofing_active;
    std::mt19937 m_rng;

    struct HOOK_INFO {
        uintptr_t original_address;
        uintptr_t hook_address;
        BYTE original_bytes[20];
        BYTE hook_bytes[20];
        bool hooked;
    };

    std::map<uintptr_t, HOOK_INFO> m_hooks;

public:
    CallbackSpoofer();
    ~CallbackSpoofer();

    bool Initialize();
    bool StartSpoofing();
    bool StopSpoofing();
    bool IsSpoofingActive() const { return m_spoofing_active.load(); }

    bool InstallHook(uintptr_t target, uintptr_t hook);
    bool RemoveHook(uintptr_t target);
    bool InstallIATHook(HMODULE module, const char* target_module, const char* function_name, uintptr_t hook);

    bool HookGetCursorPos();
    bool HookSetCursorPos();
    bool HookGetAsyncKeyState();
    bool HookMouseEvent();

private:
    bool WriteMemory(uintptr_t address, const void* data, size_t size);
    bool ReadMemory(uintptr_t address, void* buffer, size_t size);
    bool ProtectMemory(uintptr_t address, size_t size, DWORD new_protection, DWORD* old_protection = nullptr);
    bool CreateTrampoline(uintptr_t target, uintptr_t hook, HOOK_INFO& hook_info);

    void RandomDelay();
    uintptr_t GetFunctionAddress(HMODULE module, const char* function_name);
    bool ValidateMemory(uintptr_t address, size_t size = 1);
};

extern CallbackSpoofer g_CallbackSpoofer;

extern "C" {
    extern BOOL(WINAPI* OriginalGetCursorPos)(LPPOINT);
    extern BOOL(WINAPI* OriginalSetCursorPos)(int, int);
    extern SHORT(WINAPI* OriginalGetAsyncKeyState)(int);
    extern VOID(WINAPI* OriginalMouseEvent)(DWORD, DWORD, DWORD, DWORD, ULONG_PTR);
}
