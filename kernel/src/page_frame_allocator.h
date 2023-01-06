#pragma once

#include <stdint.h>
#include "bitmap.h"
#include "memory.h"
#include "efi_memory.h"

class PageFrameAllocator
{
public:
    void read_efi_memory_map(EFI_MEMORY_DESCRIPTOR* mem_map, size_t mem_map_size, size_t mem_map_desc_size);
    void free_page(void* addr);
    void free_pages(void* addr, uint64_t page_count);
    void lock_page(void* addr);
    void lock_pages(void* addr, uint64_t page_count);

    void* request_page();

    uint64_t get_free_mem();
    uint64_t get_used_mem();
    uint64_t get_reserved_mem();
    Bitmap page_bitmap;

private:
    void init_bitmap(size_t bitmap_size, void* buffer_addr);
    void reserve_page(void* addr);
    void reserve_pages(void* addr, uint64_t page_count);
    void unreserve_page(void* addr);
    void unreserve_pages(void* addr, uint64_t page_count);
};