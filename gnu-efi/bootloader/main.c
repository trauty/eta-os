#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef unsigned long long size_t;

EFI_FILE* load_file(EFI_FILE* directory, CHAR16* path, EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table)
{
	EFI_FILE* loaded_file;

	EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
	system_table->BootServices->HandleProtocol(image_handle, &gEfiLoadedImageProtocolGuid, (void**)&loaded_image); // returns loaded image

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
	system_table->BootServices->HandleProtocol(loaded_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&file_system); // returns filesystem of loaded image

	if (directory == NULL)
	{
		file_system->OpenVolume(file_system, &directory); // if patch null, then dir is root
	}

	EFI_STATUS status = directory->Open(directory, &loaded_file, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY); // tries to open dir
	if (status != EFI_SUCCESS)
	{
		return NULL;
	}
	
	return loaded_file;
}

int memcmp(const void* aprt, const void* bprt, size_t n)
{
	const unsigned char* a = aprt, *b = bprt;

	for (size_t i = 0; i < n; i++)
	{
		if (a[i] < b[i])
		{
			return -1;
		}
		else if (a[i] > b[i])
		{
			return 1;
		}
	}

	return 0;
}

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) 
{
	InitializeLib(image_handle, system_table); // sets up UEFI environment
	Print(L"LESS GO \n\r");

	EFI_FILE* kernel = load_file(NULL, L"kernel.elf", image_handle, system_table);
	if (kernel == NULL)
	{
		Print(L"Could not load Eta-OS kernel, check image. \n\r");
	}
	else
	{
		Print(L"Eta-OS kernel found successfully. \n\r");
	}

	Elf64_Ehdr header;
	{
		UINTN file_info_size;
		EFI_FILE_INFO* file_info;
		kernel->GetInfo(kernel, &gEfiFileInfoGuid, &file_info_size, NULL); // gets filesize of kernel
		system_table->BootServices->AllocatePool(EfiLoaderData, file_info_size, (void**)&file_info); // allocates enough for kernel
		kernel->GetInfo(kernel, &gEfiFileInfoGuid, &file_info_size, (void**)&file_info); // fills allocated memory

		UINTN size = sizeof(header);
		kernel->Read(kernel, &size, &header); // loads kernel into header 
	}

	if // checks if kernel header is valid
	( 
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	)
	{
		Print(L"Eta-OS kernel format is bad, check compilation or image.\n\r");
	}
	else
	{
		Print(L"Eta-OS kernel header successfully verified.\n\r");
	}

	Elf64_Phdr* p_headers; //Array struct of kernel headers
	{
		kernel->SetPosition(kernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		system_table->BootServices->AllocatePool(EfiLoaderData, size, (void**)&p_headers);
		kernel->Read(kernel, &size, p_headers);
	}

	for 
	(
		Elf64_Phdr* p_header = p_headers;
		(char*)p_header < (char*)p_headers + header.e_phnum * header.e_phentsize;
		p_header = (Elf64_Phdr*)((char*)p_header + header.e_phentsize)
	)
	{
		switch (p_header->p_type)
		{
			case PT_LOAD:
			{
				int pages = (p_header->p_memsz + 0x1000 - 1) / 0x1000; // getting memory size
				Elf64_Addr segment = p_header->p_paddr;
				system_table->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment); //alloc

				kernel->SetPosition(kernel, p_header->p_offset);
				UINTN size = p_header->p_filesz;
				kernel->Read(kernel, &size, (void*)segment);
				break;
			}
		}
	}

	Print(L"Eta-OS kernel loaded. \n\r");

	int (*kernel_start)() = ((__attribute__((sysv_abi)) int (*)() ) header.e_entry); // function pointer for kernel entry, 
																					//__attribute__ defines call conventions for compiler

	Print(L"%d \r\n", kernel_start());

	return EFI_SUCCESS; // exit the UEFI application
}