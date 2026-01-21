#include "mapper.hpp"
#include "drv.h"
#include "../../../crypt.hxx"



HANDLE Hasty_Kernel_Module;

uint64_t Hasty::AllocIndependentPages(HANDLE device_handle, uint32_t size)
{
	const auto base = intel_driver::MmAllocateIndependentPagesEx(device_handle, size);

	if (!intel_driver::MmSetPageProtection(device_handle, base, size, PAGE_EXECUTE_READWRITE))
	{
		intel_driver::MmFreeIndependentPages(device_handle, base, size);
		return 0;
	}

	return base;
}

void Hasty::Load_Dependencies() {
	Hasty_Kernel_Module = intel_driver::Load();
	if (Hasty_Kernel_Module == INVALID_HANDLE_VALUE) {
		MessageBoxA(0, hash_string("Couldn't Load Kernel Module"), (" Contact Support!"), 0);
	}
	Hasty::AllocationMode mode = Hasty::AllocationMode::AllocatePool;
	Hasty::Hasty_Loader(Hasty_Kernel_Module, module_data, 0, 0, false, true, mode, false, nullptr, 0);

	intel_driver::Unload(Hasty_Kernel_Module);
}

uint64_t Hasty::Hasty_Loader(HANDLE handle, BYTE* data, ULONG64 param1, ULONG64 param2, bool free, bool destroyHeader, AllocationMode mode, bool PassAllocationAddressAsFirstParam, mapCallback callback, NTSTATUS* exitCode) {

	const PIMAGE_NT_HEADERS64 nt_headers = portable_executable::GetNtHeaders(data);

	uint32_t image_size = nt_headers->OptionalHeader.SizeOfImage;

	void* local_image_base = VirtualAlloc(nullptr, image_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!local_image_base)
		return 0;

	DWORD TotalVirtualHeaderSize = (IMAGE_FIRST_SECTION(nt_headers))->VirtualAddress;
	image_size = image_size - (destroyHeader ? TotalVirtualHeaderSize : 0);

	uint64_t kernel_image_base = 0;
	if (mode == AllocationMode::AllocateIndependentPages) {
		kernel_image_base = AllocIndependentPages(handle, image_size);
	}
	else {
		kernel_image_base = intel_driver::AllocatePool(handle, nt::POOL_TYPE::NonPagedPool, image_size);
	}

	if (!kernel_image_base) {
		VirtualFree(local_image_base, 0, MEM_RELEASE);
		return 0;
	}

	do {
		memcpy(local_image_base, data, nt_headers->OptionalHeader.SizeOfHeaders);

		const PIMAGE_SECTION_HEADER current_image_section = IMAGE_FIRST_SECTION(nt_headers);

		for (auto i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i) {
			if ((current_image_section[i].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) > 0)
				continue;
			auto local_section = reinterpret_cast<void*>(reinterpret_cast<uint64_t>(local_image_base) + current_image_section[i].VirtualAddress);
			memcpy(local_section, reinterpret_cast<void*>(reinterpret_cast<uint64_t>(data) + current_image_section[i].PointerToRawData), current_image_section[i].SizeOfRawData);
		}

		uint64_t realBase = kernel_image_base;
		if (destroyHeader) {
			kernel_image_base -= TotalVirtualHeaderSize;
		}

		RelocateImageByDelta(portable_executable::GetRelocs(local_image_base), kernel_image_base - nt_headers->OptionalHeader.ImageBase);
		if (!FixSecurityCookie(local_image_base, kernel_image_base))
		{
			return 0;
		}

		if (!ResolveImports(handle, portable_executable::GetImports(local_image_base))) {
			kernel_image_base = realBase;
			break;
		}

		if (!intel_driver::WriteMemory(handle, realBase, (PVOID)((uintptr_t)local_image_base + (destroyHeader ? TotalVirtualHeaderSize : 0)), image_size)) {
			kernel_image_base = realBase;
			break;
		}

		const uint64_t address_of_entry_point = kernel_image_base + nt_headers->OptionalHeader.AddressOfEntryPoint;

		if (callback) {
			if (!callback(&param1, &param2, realBase, image_size)) {
				kernel_image_base = realBase;
				break;
			}
		}

		NTSTATUS status = 0;
		if (!intel_driver::CallKernelFunction(handle, &status, address_of_entry_point, (PassAllocationAddressAsFirstParam ? realBase : param1), param2)) {
			kernel_image_base = realBase;
			break;
		}

		if (exitCode) *exitCode = status;

		if (free) {
			bool free_status = false;

			if (mode == AllocationMode::AllocateIndependentPages)
			{
				free_status = intel_driver::MmFreeIndependentPages(Hasty_Kernel_Module, realBase, image_size);
			}
			else {
				free_status = intel_driver::FreePool(Hasty_Kernel_Module, realBase);
			}
		}


		VirtualFree(local_image_base, 0, MEM_RELEASE);
		return realBase;

	} while (false);


	VirtualFree(local_image_base, 0, MEM_RELEASE);

	bool free_status = false;

	if (mode == AllocationMode::AllocateIndependentPages)
	{
		free_status = intel_driver::MmFreeIndependentPages(handle, kernel_image_base, image_size);
	}
	else {
		free_status = intel_driver::FreePool(handle, kernel_image_base);
	}

	return 0;
}

void Hasty::RelocateImageByDelta(portable_executable::vec_relocs relocs, const uint64_t delta) {
	for (const auto& current_reloc : relocs) {
		for (auto i = 0u; i < current_reloc.count; ++i) {
			const uint16_t type = current_reloc.item[i] >> 12;
			const uint16_t offset = current_reloc.item[i] & 0xFFF;

			if (type == IMAGE_REL_BASED_DIR64)
				*reinterpret_cast<uint64_t*>(current_reloc.address + offset) += delta;
		}
	}
}


bool Hasty::FixSecurityCookie(void* local_image, uint64_t kernel_image_base)
{
	auto headers = portable_executable::GetNtHeaders(local_image);
	if (!headers)
		return false;

	auto load_config_directory = headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress;
	if (!load_config_directory)
	{
		return true;
	}

	auto load_config_struct = (PIMAGE_LOAD_CONFIG_DIRECTORY)((uintptr_t)local_image + load_config_directory);
	auto stack_cookie = load_config_struct->SecurityCookie;
	if (!stack_cookie)
	{
		return true; // as I said, it is not an error and we should allow that behavior
	}

	stack_cookie = stack_cookie - (uintptr_t)kernel_image_base + (uintptr_t)local_image; //since our local image is already relocated the base returned will be kernel address

	if (*(uintptr_t*)(stack_cookie) != 0x2B992DDFA232) {
		return false;
	}

	auto new_cookie = 0x2B992DDFA232 ^ GetCurrentProcessId() ^ GetCurrentThreadId(); // here we don't really care about the value of stack cookie, it will still works and produce nice result
	if (new_cookie == 0x2B992DDFA232)
		new_cookie = 0x2B992DDFA233;

	*(uintptr_t*)(stack_cookie) = new_cookie; // the _security_cookie_complement will be init by the driver itself if they use crt
	return true;
}

bool Hasty::ResolveImports(HANDLE iqvw64e_device_handle, portable_executable::vec_imports imports) {
	for (const auto& current_import : imports) {
		ULONG64 Module = eterna_utils::GetKernelModuleAddress(current_import.module_name);
		if (!Module) return false;

		for (auto& current_function_data : current_import.function_datas) {
			uint64_t function_address = intel_driver::GetKernelModuleExport(iqvw64e_device_handle, Module, current_function_data.name);

			if (!function_address) {
				if (Module != intel_driver::ntoskrnlAddr) {
					function_address = intel_driver::GetKernelModuleExport(iqvw64e_device_handle, intel_driver::ntoskrnlAddr, current_function_data.name);
					if (!function_address) {
						return false;
					}
				}
			}
			*current_function_data.address = function_address;
		}
	}

	return true;
}
