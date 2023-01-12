#pragma once

#include <stddef.h>

#include "c_str.h"
#include "memory.h"
#include "bitmap.h"
#include "efi_memory.h"
#include "basic_renderer.h"
#include "paging/paging.h"
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
};

struct KernelInfo
{
    PageTableManager* page_table_mgr;
};

KernelInfo init_kernel(BOOT_INFO* boot_info);

extern uint64_t _kernel_start;
extern uint64_t _kernel_end;