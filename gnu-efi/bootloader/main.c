#include <efi.h>
#include <efilib.h>
#include <elf.h>

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef unsigned long long size_t;

typedef struct
{
	void* base_adress;
	size_t buffer_size;
	unsigned int width;
	unsigned int height;
	unsigned int pixels_per_scanline;
}Framebuffer;

typedef struct
{
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
}PSF1_HEADER;

typedef struct
{
	PSF1_HEADER* psf1_header;
	void* glyph_buffer;
}PSF1_FONT;

Framebuffer framebuffer;

Framebuffer* init_gop()
{
	EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;
	
	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop); // calling convention for UEFI
	if (EFI_ERROR(status))
	{
		Print(L"Unable to locate GOP.\n\r");
		return NULL;
	}
	else
	{
		Print(L"Located GOP.\n\r");
	}

	framebuffer.base_adress = (void*)gop->Mode->FrameBufferBase;
	framebuffer.buffer_size = gop->Mode->FrameBufferSize;
	framebuffer.width = gop->Mode->Info->HorizontalResolution;
	framebuffer.height = gop->Mode->Info->VerticalResolution;
	framebuffer.pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;

	return &framebuffer;
}

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

PSF1_FONT* load_psf1_font(EFI_FILE* directory, CHAR16* path, EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table)
{
	EFI_FILE* font = load_file(directory, path, image_handle, system_table); // loads font file

	if (font == NULL) { return NULL; }

	PSF1_HEADER* font_header;
	system_table->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&font_header);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font, &size, font_header);

	if (font_header->magic[0] != PSF1_MAGIC0 || font_header->magic[1] != PSF1_MAGIC1)
	{
		return NULL;
	}

	UINTN glyph_buffer_size = font_header->mode == 1 ? font_header->charsize * 512 : font_header->charsize * 256;

	void* glyph_buffer;
	{
		font->SetPosition(font, sizeof(PSF1_HEADER));
		system_table->BootServices->AllocatePool(EfiLoaderData, glyph_buffer_size, (void**)&glyph_buffer);
		font->Read(font, &glyph_buffer_size, glyph_buffer);
	}

	PSF1_FONT* final_font;
	system_table->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void**)&final_font);
	final_font->psf1_header = font_header;
	final_font->glyph_buffer = glyph_buffer;

	return final_font;
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

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) 
{
	InitializeLib(image_handle, system_table); // sets up UEFI environment
	Print(L"LESS GO \n\r");

	EFI_FILE* kernel = load_file(NULL, L"kernel.elf", image_handle, system_table); // loads kernel elf file
	
	if (kernel == NULL)
	{
		Print(L"Could not load Eta-OS kernel, check image. \n\r");
	}
	else
	{
		Print(L"Eta-OS kernel found successfully. \n\r");
	}

	Elf64_Ehdr header; // defines kernel header
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
			case PT_LOAD: // if segment has to be loaded in
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

	void (*kernel_start)(Framebuffer*, PSF1_FONT*) = ((__attribute__((sysv_abi)) void (*)(Framebuffer*, PSF1_FONT*)) header.e_entry); // function pointer for kernel entry, //__attribute__ defines call conventions for compiler

	PSF1_FONT* new_font = load_psf1_font(NULL, L"zap-light16.psf", image_handle, system_table);

	if (new_font == NULL)
	{
		Print(L"Main font not found or is not valid!\r\n");
	}
	else
	{
		Print(L"Font found, char size = %d.\r\n", new_font->psf1_header->charsize);
	}

	Framebuffer* new_buffer = init_gop(); // inits framebuffer

	Print(L"Base: 0x%x\r\nSize: 0x%x\r\nWidth: %d\r\nHeight: %d\r\nPixels per scanline: %d\r\n",
		new_buffer->base_adress,
		new_buffer->buffer_size,
		new_buffer->width,
		new_buffer->height,
		new_buffer->pixels_per_scanline);

	kernel_start(new_buffer, new_font); // calls entry function of kernel and passes pointer to framebuffer

	return EFI_SUCCESS; // exit the UEFI application
}