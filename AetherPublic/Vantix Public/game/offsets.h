#define _CRT_SECURE_NO_WARNINGS

#pragma once
#include "memory/driver.h"
#include <unordered_map>
#include "../crypt.hxx"

//const int start = 0xkurwa;
//const int end_offsetxd = 0x10000;
//const int step = 1;                // 4, 8, albo 16
//
//for (int offset = start; offset <= end_offsetxd; offset += step) {
//    int readujkurwe = io->read<int>(CachePointers.AcknownledgedPawn + offset); //w twoim przypadku player state i offset to team index
//
//    if (readujkurwe != 1 && readujkurwe != 3) {
//        continue;
//    }
//
//    std::cout << "value offset 0x" << std::hex << offset
//        << ": " << readujkurwe << std::dec << std::endl;
//    //if (readujkurwe == 1 && 3) {
//    //    std::cout << "Correct offset found (Value = 1 and 3): 0x" << (int)offset << std::endl;
//    //    break;
//    //}
//}

void save_offset(const char* section, const char* key, uintptr_t value, const char* path) {
    char buffer[1024];
    sprintf(buffer, "0x%zX", value);  // Format uintptr_t as a hexadecimal string
    WritePrivateProfileStringA(section, key, buffer, path);
}


static BOOL read_offset(LPCSTR section, LPCSTR key, uintptr_t& target, LPCSTR path) {
    char buffer[1024];
    if (GetPrivateProfileStringA(section, key, "", buffer, sizeof(buffer), path)) {
        char* endPtr = nullptr;
        uintptr_t value = static_cast<uintptr_t>(strtoull(buffer, &endPtr, 0));  // Use base 0 to auto-detect base
        if (endPtr != buffer && *endPtr == '\0') {  // Ensure successful parsing
            target = value;
            return true;
        }
    }
    return false;
}


namespace offsets
{
    inline static uintptr_t UWorld = 0x16EA99B0;
    inline static uintptr_t OwningGameInstance = 0x240;
    inline static uintptr_t GameState = 0x1C8;
    inline static uintptr_t PlayerController = 0x30;
    inline static uintptr_t LocalPlayers = 0x38;
    inline static uintptr_t BoneArray = 0x5E8;
    inline static  uintptr_t PlayerCameraManager = 0x3D0;
    inline static uintptr_t AcknowledgedPawn = 0x358;
    inline static uintptr_t PlayerArray = 0x2C8;
    inline static uintptr_t PlayerState = 0x2D0;
    inline static uintptr_t OwningWorld = 0x118;
    inline static uintptr_t PersistentLevel = 0x38;
    inline static uintptr_t ComponentToWorld = 0x1E0;
    inline static uintptr_t Mesh = 0x330;
    inline static uintptr_t RootComponent = 0x1B0;
    inline static uintptr_t PawnPrivate = 0x328;
    inline static uintptr_t Name = 0x167B6600;  // this is gnames idk why it said only Name in yr list thats why yr wrong i think 
    inline static uintptr_t weapondata = 0x5d0;
    inline static uintptr_t Platform = 0x440;
    inline static uintptr_t CurrentWeapon = 0x990;
    inline static uintptr_t fLastSubmitTime = 0x198; // updated this also
    inline static uintptr_t fLastRenderTimeOnScreen = 0x32C;
    inline static uintptr_t TargetedFortPawn = 0x1900;
    inline static uintptr_t Team_Index = 0x11A9;
    inline static uintptr_t Velocity = 0x188;
    inline static uintptr_t speed = 0x212C;
    inline static uintptr_t gravity = 0x2130;
    inline static uintptr_t RelativeLocation = 0x140;
    inline static uintptr_t b_ads_while_not_on_ground = 0x5869;


}

LPCSTR path = "C:\\offsets.ini";

void saveoffsets()
{
    save_offset(hash_string("Offsets"), hash_string("Uworld"), offsets::UWorld, path);
    save_offset(hash_string("Offsets"), hash_string("OwningGameInstance"), offsets::OwningGameInstance, path);
    save_offset(hash_string("Offsets"), hash_string("GameState"), offsets::GameState, path);
    save_offset(hash_string("Offsets"), hash_string("PlayerController"), offsets::PlayerController, path);
    save_offset(hash_string("Offsets"), hash_string("LocalPlayers"), offsets::LocalPlayers, path);
    save_offset(hash_string("Offsets"), hash_string("BoneArray"), offsets::BoneArray, path);
    save_offset(hash_string("Offsets"), hash_string("AcknowledgedPawn"), offsets::AcknowledgedPawn, path);
    save_offset(hash_string("Offsets"), hash_string("PlayerArray"), offsets::PlayerArray, path);
    save_offset(hash_string("Offsets"), hash_string("PlayerState"), offsets::PlayerState, path);
    save_offset(hash_string("Offsets"), hash_string("OwningWorld"), offsets::OwningWorld, path);
    save_offset(hash_string("Offsets"), hash_string("PersistentLevel"), offsets::PersistentLevel, path);
    save_offset(hash_string("Offsets"), hash_string("ComponentToWorld"), offsets::ComponentToWorld, path);
    save_offset(hash_string("Offsets"), hash_string("Mesh"), offsets::Mesh, path);
    save_offset(hash_string("Offsets"), hash_string("RootComponent"), offsets::RootComponent, path);
    save_offset(hash_string("Offsets"), hash_string("PawnPrivate"), offsets::PawnPrivate, path);
    save_offset(hash_string("Offsets"), hash_string("Name"), offsets::Name, path);
    save_offset(hash_string("Offsets"), hash_string("weapondata"), offsets::weapondata, path);
    save_offset(hash_string("Offsets"), hash_string("Platform"), offsets::Platform, path);
    save_offset(hash_string("Offsets"), hash_string("CurrentWeapon"), offsets::CurrentWeapon, path);
    save_offset(hash_string("Offsets"), hash_string("UfLastSubmitTimeworld"), offsets::fLastSubmitTime, path);
    save_offset(hash_string("Offsets"), hash_string("fLastRenderTimeOnScreen"), offsets::fLastRenderTimeOnScreen, path);
    save_offset(hash_string("Offsets"), hash_string("TargetedFortPawn"), offsets::TargetedFortPawn, path);
    save_offset(hash_string("Offsets"), hash_string("Team_Index"), offsets::Team_Index, path);
    save_offset(hash_string("Offsets"), hash_string("Velocity"), offsets::Velocity, path);
    save_offset(hash_string("Offsets"), hash_string("speed"), offsets::speed, path);
    save_offset(hash_string("Offsets"), hash_string("gravity"), offsets::gravity, path);
    save_offset(hash_string("Offsets"), hash_string("relativeloc"), offsets::RelativeLocation, path);
}

void loadoffsets()
{
    read_offset(hash_string("Offsets"), hash_string("Uworld"), offsets::UWorld, path);
    read_offset(hash_string("Offsets"), hash_string("OwningGameInstance"), offsets::OwningGameInstance, path);
    read_offset(hash_string("Offsets"), hash_string("GameState"), offsets::GameState, path);
    read_offset(hash_string("Offsets"), hash_string("PlayerController"), offsets::PlayerController, path);
    read_offset(hash_string("Offsets"), hash_string("LocalPlayers"), offsets::LocalPlayers, path);
    read_offset(hash_string("Offsets"), hash_string("BoneArray"), offsets::BoneArray, path);
    read_offset(hash_string("Offsets"), hash_string("AcknowledgedPawn"), offsets::AcknowledgedPawn, path);
    read_offset(hash_string("Offsets"), hash_string("PlayerArray"), offsets::PlayerArray, path);
    read_offset(hash_string("Offsets"), hash_string("PlayerState"), offsets::PlayerState, path);
    read_offset(hash_string("Offsets"), hash_string("OwningWorld"), offsets::OwningWorld, path);
    read_offset(hash_string("Offsets"), hash_string("PersistentLevel"), offsets::PersistentLevel, path);
    read_offset(hash_string("Offsets"), hash_string("ComponentToWorld"), offsets::ComponentToWorld, path);
    read_offset(hash_string("Offsets"), hash_string("Mesh"), offsets::Mesh, path);
    read_offset(hash_string("Offsets"), hash_string("RootComponent"), offsets::RootComponent, path);
    read_offset(hash_string("Offsets"), hash_string("PawnPrivate"), offsets::PawnPrivate, path);
    read_offset(hash_string("Offsets"), hash_string("Name"), offsets::Name, path);
    read_offset(hash_string("Offsets"), hash_string("weapondata"), offsets::weapondata, path);
    read_offset(hash_string("Offsets"), hash_string("Platform"), offsets::Platform, path);
    read_offset(hash_string("Offsets"), hash_string("CurrentWeapon"), offsets::CurrentWeapon, path);
    read_offset(hash_string("Offsets"), hash_string("UfLastSubmitTimeworld"), offsets::fLastSubmitTime, path);
    read_offset(hash_string("Offsets"), hash_string("fLastRenderTimeOnScreen"), offsets::fLastRenderTimeOnScreen, path);
    read_offset(hash_string("Offsets"), hash_string("TargetedFortPawn"), offsets::TargetedFortPawn, path);
    read_offset(hash_string("Offsets"), hash_string("Team_Index"), offsets::Team_Index, path);
    read_offset(hash_string("Offsets"), hash_string("Velocity"), offsets::Velocity, path);
    read_offset(hash_string("Offsets"), hash_string("speed"), offsets::speed, path);
    read_offset(hash_string("Offsets"), hash_string("gravity"), offsets::gravity, path);
    read_offset(hash_string("Offsets"), hash_string("relativeloc"), offsets::RelativeLocation, path);
}
struct TArray {
    uintptr_t Array = 0;
    uint32_t Count = 0;
    uint32_t MaxCount = 0;

    uintptr_t Get(uint32_t Index) const {
        if (Index >= Count) {
            return 0;
        }
        return read<uintptr_t>(Array + (Index * sizeof(uintptr_t)));
    }

    uint32_t size() const {
        return Count;
    }

    uintptr_t operator[](uint32_t Index) const {
        return Get(Index);
    }

    bool isValid() const {
        return Array && Count <= MaxCount && MaxCount <= 1000000;
    }

    uintptr_t getAddress() const {
        return Array;
    }
};
