#pragma once

#include <stddef.h>

#include "io.h"
#include "pci.h"
#include "acpi.h"
#include "gdt/gdt.h"
#include "c_str.h"
#include "memory.h"
#include "bitmap.h"
#include "efi_memory.h"
#include "interrupts/idt.h"
#include "interrupts/interrupts.h"
#include "basic_renderer.h"
#include "paging/paging.h"
#include "memory/heap.h"
#include "paging/page_map_indexer.h"
#include "paging/page_table_manager.h"
#include "paging/page_frame_allocator.h"

struct BOOT_INFO
{
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
	void* mem_map;
	uint64_t mem_map_size;
	uint64_t mem_map_descriptor_size;
	ACPI::RSDP2* rsdp;
};

struct KernelInfo
{
    PageTableManager* page_table_mgr;
};

KernelInfo init_kernel(BOOT_INFO* boot_info);

extern uint64_t _kernel_start;
extern uint64_t _kernel_end;