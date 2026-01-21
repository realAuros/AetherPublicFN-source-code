#pragma once

#include <Windows.h>
#include <cstdint>
#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <cstring>
#include "../../crypt.hxx"

inline uintptr_t virtualaddy;

#define c_read     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3650, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define c_base     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4321, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define c_cr3      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x5190, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define c_protect  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3A5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define c_security 0x8CDE4B2

typedef struct _rw {
	INT32 security;
	INT32 process_id;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
	BOOLEAN write;
} rw, * prw;

typedef struct sBaseAddress {
	INT32 security;
	INT32 process_id;
	ULONGLONG* address;
} ba, * pba;

typedef struct _MEMORY_OPERATION_DATA {
	uint32_t        pid;
	ULONGLONG* cr3;
} MEMORY_OPERATION_DATA, * PMEMORY_OPERATION_DATA;

typedef struct _TARGET_PROCESS {
	int ProcessId;
} TARGET_PROCESS, * PTARGET_PROCESS;

#include <winternl.h>

extern "C" NTSTATUS NTAPI NtCreateFile(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	PLARGE_INTEGER AllocationSize,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions,
	PVOID EaBuffer,
	ULONG EaLength
);

using NtCreateFile_t = NTSTATUS(NTAPI*)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	PLARGE_INTEGER AllocationSize,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions,
	PVOID EaBuffer,
	ULONG EaLength
	);


namespace mem {
	inline HANDLE driver_handle;
	inline INT32 process_id;

	inline bool init() {
		UNICODE_STRING dev_name;
		OBJECT_ATTRIBUTES attr;
		IO_STATUS_BLOCK io = { 0 };

		RtlInitUnicodeString(&dev_name, L"\\Device\\Null");
		InitializeObjectAttributes(&attr, &dev_name, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

		HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
		if (!ntdll) return false;

		auto NtCreateFile = reinterpret_cast<NtCreateFile_t>(
			GetProcAddress(ntdll, "NtCreateFile")
			);

		if (!NtCreateFile) return false;

		NTSTATUS status = NtCreateFile(
			&driver_handle,
			GENERIC_READ | GENERIC_WRITE,
			&attr,
			&io,
			nullptr,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			FILE_OPEN,
			0,
			nullptr,
			0
		);

		if (!NT_SUCCESS(status)) {
			printf("NtCreateFile failed: 0x%X\n", status);
			return false;
		}
		printf("Opened handle: 0x%p\n", driver_handle);


		return NT_SUCCESS(status);
	}

	inline uintptr_t get_pid(LPCTSTR process_name)
	{
		PROCESSENTRY32 pt;
		HANDLE hsnap = (CreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0);
		pt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hsnap, &pt)) {
			do {
				if (!lstrcmpi(pt.szExeFile, process_name)) {
					(CloseHandle)(hsnap);
					process_id = pt.th32ProcessID;
					return pt.th32ProcessID;
				}
			} while (Process32Next(hsnap, &pt));
		} (CloseHandle)(hsnap); return { NULL };
	}

	inline uintptr_t base_address() {
		uintptr_t image_address = { NULL };
		sBaseAddress arguments = { NULL };

		arguments.security = c_security;
		arguments.process_id = process_id;
		arguments.address = (ULONGLONG*)&image_address;

		DeviceIoControl(driver_handle, c_base, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return image_address;
	}

	inline void read_physical(uintptr_t address, PVOID buffer, DWORD size) {
		_rw arguments = { 0 };

		arguments.security = c_security;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = FALSE;

		DeviceIoControl(driver_handle, c_read, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	inline void write_physical(PVOID address, PVOID buffer, DWORD size) {
		_rw arguments = { 0 };

		arguments.security = c_security;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = TRUE;

		DeviceIoControl(driver_handle, c_read, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	inline uintptr_t fetch_cr3() {
		_MEMORY_OPERATION_DATA arguments = { 0 };

		arguments.pid = process_id;
		DeviceIoControl(driver_handle, c_cr3, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return true;
	}

}

template <typename T>
T read(uint64_t address) {
	T buffer{ };
	mem::read_physical(address, &buffer, sizeof(T));
	return buffer;
}

template<typename T>
bool read_array(uintptr_t address, T out[], size_t len)
{
	for (size_t i = 0; i < len; ++i)
	{
		out[i] = read<T>(address + i * sizeof(T));
	}
	return true;
}

template <typename T>
T write(uint64_t address, T buffer) {

	mem::write_physical((PVOID)address, &buffer, sizeof(T));
	return buffer;
}

inline bool is_valid(const uint64_t adress)
{
	if (adress <= 0x400000 || adress == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(adress) == nullptr || adress >
		0x7FFFFFFFFFFFFFFF) {
		return false;
	}
	return true;
}

template <typename T>
std::vector<T> batch_read(const std::vector<uint64_t>& addresses) {
	size_t num_addresses = addresses.size();
	std::vector<T> results(num_addresses);
	std::vector<uint8_t> buffer(num_addresses * sizeof(T));

	for (size_t i = 0; i < num_addresses; ++i) {
		mem::read_physical((PVOID)addresses[i], buffer.data() + i * sizeof(T), sizeof(T));
	}

	for (size_t i = 0; i < num_addresses; ++i) {
		results[i] = *reinterpret_cast<T*>(buffer.data() + i * sizeof(T));
	}

	return results;
}


template <typename T>
void write_bit(uint64_t address, uint8_t bitPosition, T value)
{
	uint8_t byteAtOffset = read<uint8_t>(address);

	if (value) {
		byteAtOffset |= (1 << bitPosition);
	}
	else {
		byteAtOffset &= ~(1 << bitPosition);
	}
	write<uint8_t>(address, byteAtOffset);
}
