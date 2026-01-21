#include "callbackspoofer.h"
#include <iostream>
#include <thread>
#include <TlHelp32.h>
#include <Psapi.h>

BOOL(WINAPI* OriginalGetCursorPos)(LPPOINT) = nullptr;
BOOL(WINAPI* OriginalSetCursorPos)(int, int) = nullptr;
SHORT(WINAPI* OriginalGetAsyncKeyState)(int) = nullptr;
VOID(WINAPI* OriginalMouseEvent)(DWORD, DWORD, DWORD, DWORD, ULONG_PTR) = nullptr;

CallbackSpoofer g_CallbackSpoofer;

BOOL WINAPI HookedGetCursorPos(LPPOINT lpPoint) {
    if (!g_CallbackSpoofer.IsSpoofingActive() || !OriginalGetCursorPos) {
        return GetCursorPos(lpPoint);
    }

    BOOL result = OriginalGetCursorPos(lpPoint);
    if (result && lpPoint) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<int> dist(-3, 3);

        lpPoint->x += dist(gen);
        lpPoint->y += dist(gen);
    }
    return result;
}

BOOL WINAPI HookedSetCursorPos(int X, int Y) {
    if (!g_CallbackSpoofer.IsSpoofingActive() || !OriginalSetCursorPos) {
        return SetCursorPos(X, Y);
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(-2, 2);

    int newX = X + dist(gen);
    int newY = Y + dist(gen);

    return OriginalSetCursorPos(newX, newY);
}

SHORT WINAPI HookedGetAsyncKeyState(int vKey) {
    if (!g_CallbackSpoofer.IsSpoofingActive() || !OriginalGetAsyncKeyState) {
        return GetAsyncKeyState(vKey);
    }

    SHORT state = OriginalGetAsyncKeyState(vKey);

    if (vKey == VK_SHIFT || vKey == VK_CONTROL || vKey == VK_MENU ||
        vKey == VK_LBUTTON || vKey == VK_RBUTTON) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::bernoulli_distribution dist(0.8);

        if (dist(gen)) {
            state = 0;
        }
    }

    return state;
}

VOID WINAPI HookedMouseEvent(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo) {
    if (!g_CallbackSpoofer.IsSpoofingActive() || !OriginalMouseEvent) {
        mouse_event(dwFlags, dx, dy, dwData, dwExtraInfo);
        return;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<DWORD> pos_dist(0, 5);

    DWORD newDx = dx + pos_dist(gen);
    DWORD newDy = dy + pos_dist(gen);

    OriginalMouseEvent(dwFlags, newDx, newDy, dwData, dwExtraInfo);
}

CallbackSpoofer::CallbackSpoofer()
    : m_initialized(false)
    , m_spoofing_active(false)
    , m_rng(std::random_device{}())
{
}

CallbackSpoofer::~CallbackSpoofer() {
    StopSpoofing();
}

bool CallbackSpoofer::Initialize() {
    if (m_initialized) return true;

    HMODULE user32 = GetModuleHandleA("user32.dll");
    if (!user32) return false;

    OriginalGetCursorPos = (BOOL(WINAPI*)(LPPOINT))GetProcAddress(user32, "GetCursorPos");
    OriginalSetCursorPos = (BOOL(WINAPI*)(int, int))GetProcAddress(user32, "SetCursorPos");
    OriginalGetAsyncKeyState = (SHORT(WINAPI*)(int))GetProcAddress(user32, "GetAsyncKeyState");
    OriginalMouseEvent = (VOID(WINAPI*)(DWORD, DWORD, DWORD, DWORD, ULONG_PTR))GetProcAddress(user32, "mouse_event");

    m_initialized = true;
    return true;
}

bool CallbackSpoofer::StartSpoofing() {
    if (!m_initialized && !Initialize()) {
        return false;
    }

    if (m_spoofing_active) return true;

    bool success = true;

    success &= HookGetCursorPos();
    RandomDelay();

    success &= HookSetCursorPos();
    RandomDelay();

    success &= HookGetAsyncKeyState();
    RandomDelay();

    success &= HookMouseEvent();

    m_spoofing_active = success;
    return success;
}

bool CallbackSpoofer::StopSpoofing() {
    if (!m_spoofing_active) return true;

    for (auto& hook : m_hooks) {
        RemoveHook(hook.first);
    }

    m_hooks.clear();
    m_spoofing_active = false;
    return true;
}

bool CallbackSpoofer::InstallHook(uintptr_t target, uintptr_t hook) {
    if (!ValidateMemory(target) || !ValidateMemory(hook)) {
        return false;
    }

    HOOK_INFO hook_info;
    hook_info.original_address = target;
    hook_info.hook_address = hook;
    hook_info.hooked = false;

    if (!CreateTrampoline(target, hook, hook_info)) {
        return false;
    }

    m_hooks[target] = hook_info;
    return true;
}

bool CallbackSpoofer::RemoveHook(uintptr_t target) {
    auto it = m_hooks.find(target);
    if (it == m_hooks.end()) {
        return false;
    }

    HOOK_INFO& hook_info = it->second;

    if (hook_info.hooked) {
        DWORD old_protect;
        if (ProtectMemory(hook_info.original_address, sizeof(hook_info.original_bytes), PAGE_EXECUTE_READWRITE, &old_protect)) {
            WriteMemory(hook_info.original_address, hook_info.original_bytes, sizeof(hook_info.original_bytes));
            ProtectMemory(hook_info.original_address, sizeof(hook_info.original_bytes), old_protect);
        }
    }

    m_hooks.erase(it);
    return true;
}

bool CallbackSpoofer::CreateTrampoline(uintptr_t target, uintptr_t hook, HOOK_INFO& hook_info) {
    if (!ReadMemory(target, hook_info.original_bytes, sizeof(hook_info.original_bytes))) {
        return false;
    }


    BYTE jump_code[] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    *(uintptr_t*)(jump_code + 6) = hook;

    DWORD old_protect;
    if (!ProtectMemory(target, sizeof(jump_code), PAGE_EXECUTE_READWRITE, &old_protect)) {
        return false;
    }

    if (!WriteMemory(target, jump_code, sizeof(jump_code))) {
        ProtectMemory(target, sizeof(jump_code), old_protect);
        return false;
    }

    ProtectMemory(target, sizeof(jump_code), old_protect);

    hook_info.hooked = true;
    memcpy(hook_info.hook_bytes, jump_code, sizeof(jump_code));

    return true;
}

bool CallbackSpoofer::HookGetCursorPos() {
    if (!OriginalGetCursorPos) return false;

    return InstallHook((uintptr_t)OriginalGetCursorPos, (uintptr_t)HookedGetCursorPos);
}

bool CallbackSpoofer::HookSetCursorPos() {
    if (!OriginalSetCursorPos) return false;

    return InstallHook((uintptr_t)OriginalSetCursorPos, (uintptr_t)HookedSetCursorPos);
}

bool CallbackSpoofer::HookGetAsyncKeyState() {
    if (!OriginalGetAsyncKeyState) return false;

    return InstallHook((uintptr_t)OriginalGetAsyncKeyState, (uintptr_t)HookedGetAsyncKeyState);
}

bool CallbackSpoofer::HookMouseEvent() {
    if (!OriginalMouseEvent) return false;

    return InstallHook((uintptr_t)OriginalMouseEvent, (uintptr_t)HookedMouseEvent);
}

bool CallbackSpoofer::WriteMemory(uintptr_t address, const void* data, size_t size) {
    if (!ValidateMemory(address, size)) return false;

    memcpy((void*)address, data, size);
    return true;
}

bool CallbackSpoofer::ReadMemory(uintptr_t address, void* buffer, size_t size) {
    if (!ValidateMemory(address, size)) return false;

    memcpy(buffer, (void*)address, size);
    return true;
}

bool CallbackSpoofer::ProtectMemory(uintptr_t address, size_t size, DWORD new_protection, DWORD* old_protection) {
    if (!ValidateMemory(address, size)) return false;

    DWORD old_protect;
    if (!VirtualProtect((LPVOID)address, size, new_protection, &old_protect)) {
        return false;
    }

    if (old_protection) {
        *old_protection = old_protect;
    }

    return true;
}

bool CallbackSpoofer::ValidateMemory(uintptr_t address, size_t size) {
    if (address == 0) return false;

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery((LPCVOID)address, &mbi, sizeof(mbi))) {
        if (mbi.State != MEM_COMMIT) return false;
        if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;

        if ((address + size) > ((uintptr_t)mbi.BaseAddress + mbi.RegionSize)) {
            return false;
        }

        return true;
    }

    return false;
}

uintptr_t CallbackSpoofer::GetFunctionAddress(HMODULE module, const char* function_name) {
    if (!module) return 0;

    return (uintptr_t)GetProcAddress(module, function_name);
}

void CallbackSpoofer::RandomDelay() {
    std::uniform_int_distribution<int> delay_dist(1, 5);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_dist(m_rng)));
}

bool CallbackSpoofer::InstallIATHook(HMODULE module, const char* target_module, const char* function_name, uintptr_t hook) {
    if (!module || !target_module || !function_name) return false;

    PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)module;
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) return false;

    PIMAGE_NT_HEADERS nt_headers = (PIMAGE_NT_HEADERS)((uintptr_t)module + dos_header->e_lfanew);
    if (nt_headers->Signature != IMAGE_NT_SIGNATURE) return false;

    PIMAGE_IMPORT_DESCRIPTOR import_descriptor = (PIMAGE_IMPORT_DESCRIPTOR)(
        (uintptr_t)module + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    for (; import_descriptor->Name; import_descriptor++) {
        const char* module_name = (const char*)((uintptr_t)module + import_descriptor->Name);
        if (_stricmp(module_name, target_module) != 0) continue;

        PIMAGE_THUNK_DATA orig_thunk = (PIMAGE_THUNK_DATA)((uintptr_t)module + import_descriptor->OriginalFirstThunk);
        PIMAGE_THUNK_DATA first_thunk = (PIMAGE_THUNK_DATA)((uintptr_t)module + import_descriptor->FirstThunk);

        for (; orig_thunk->u1.AddressOfData; orig_thunk++, first_thunk++) {
            if (orig_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;

            PIMAGE_IMPORT_BY_NAME import_by_name = (PIMAGE_IMPORT_BY_NAME)((uintptr_t)module + orig_thunk->u1.AddressOfData);
            if (_stricmp((const char*)import_by_name->Name, function_name) == 0) {
                DWORD old_protect;
                if (ProtectMemory((uintptr_t)&first_thunk->u1.Function, sizeof(uintptr_t), PAGE_READWRITE, &old_protect)) {
                    uintptr_t original = first_thunk->u1.Function;
                    first_thunk->u1.Function = hook;
                    ProtectMemory((uintptr_t)&first_thunk->u1.Function, sizeof(uintptr_t), old_protect);
                    return true;
                }
            }
        }
    }

    return false;
}