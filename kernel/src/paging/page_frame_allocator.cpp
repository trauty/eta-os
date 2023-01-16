#include "page_frame_allocator.h"

uint64_t free_memory;
uint64_t reserved_memory;
uint64_t used_memory;
bool initialized = false;

PageFrameAllocator global_allocator;

void PageFrameAllocator::read_efi_memory_map(EFI_MEMORY_DESCRIPTOR* mem_map, size_t mem_map_size, size_t mem_map_desc_size)
{
    if (initialized) { return; }

    initialized = true;

    uint64_t mem_map_entries = mem_map_size / mem_map_desc_size;

    void* largest_free_mem_seg = NULL;
    size_t largest_free_mem_seg_size = 0;

    for (int i = 0; i < mem_map_entries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mem_map + (i * mem_map_desc_size));
        if (desc->type == 7) // EfiConeventionalMemory
        {
            if (desc->num_pages * 4096 > largest_free_mem_seg_size)
            {
                largest_free_mem_seg = desc->phys_addr;
                largest_free_mem_seg_size = desc->num_pages * 4096;
            }
        }
    }

    uint64_t mem_size = get_memory_size(mem_map, mem_map_entries, mem_map_desc_size);
    free_memory = mem_size;

    uint64_t bitmap_size = mem_size / 4096 / 8 + 1; // dividing per page, per byte

    init_bitmap(bitmap_size, largest_free_mem_seg);

    lock_pages(page_bitmap.buffer, page_bitmap.size / 4096 + 1); // lock bitmap pages to avoid corruption

    for (int i = 0; i < mem_map_entries; i++) // reserving memory for unusable/acpireclaimmem
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mem_map + (i * mem_map_desc_size));
        if (desc->type != 7) // not EfiConeventionalMemory
        {
            reserve_pages(desc->phys_addr, desc->num_pages);
        }
    }
}

void PageFrameAllocator::init_bitmap(size_t bitmap_size, void* buffer_addr)
{
    page_bitmap.size = bitmap_size;
    page_bitmap.buffer = (uint8_t*)buffer_addr;
    
    for (int i = 0; i < bitmap_size; i++)
    {
        *(page_bitmap.buffer + i) = 0; // experimental
    }
}

uint64_t page_bitmap_index = 0;

void* PageFrameAllocator::request_page()
{
    for (; page_bitmap_index < page_bitmap.size; page_bitmap_index++)
    {
        if (page_bitmap[page_bitmap_index] == true) { continue; }
        lock_page((void*)(page_bitmap_index * 4096));
        return (void*)(page_bitmap_index * 4096);
    }

    return NULL; // page frame swapping, could crash in the future
}

void PageFrameAllocator::free_page(void* addr)
{
    uint64_t index = (uint64_t)addr / 4096;
    
    if (page_bitmap[index] == false) { return; }
    if (page_bitmap.set(index, false))
    {
        free_memory += 4096;
        used_memory -= 4096;
        if (page_bitmap_index > index) { page_bitmap_index = index; }
    }
}

void PageFrameAllocator::free_pages(void* addr, uint64_t page_count)
{
    for (int i = 0; i < page_count; i++)
    {
        free_page((void*)((uint64_t)addr + (i * 4096)));
    }
}

void PageFrameAllocator::lock_page(void* addr)
{
    uint64_t index = (uint64_t)addr / 4096;
    
    if (page_bitmap[index] == true) { return; }

    if (page_bitmap.set(index, true))
    {
        free_memory -= 4096;
        used_memory += 4096;
    }
}

void PageFrameAllocator::lock_pages(void* addr, uint64_t page_count)
{
    for (int i = 0; i < page_count; i++)
    {
        lock_page((void*)((uint64_t)addr + (i * 4096)));
    }
}

void PageFrameAllocator::unreserve_page(void* addr)
{
    uint64_t index = (uint64_t)addr / 4096;
    
    if (page_bitmap[index] == false) { return; }

    if (page_bitmap.set(index, false))
    {
        free_memory += 4096;
        reserved_memory -= 4096;
        if (page_bitmap_index > index) { page_bitmap_index = index; }
    }
}

void PageFrameAllocator::unreserve_pages(void* addr, uint64_t page_count)
{
    for (int i = 0; i < page_count; i++)
    {
        unreserve_page((void*)((uint64_t)addr + (i * 4096)));
    }
}

void PageFrameAllocator::reserve_page(void* addr)
{
    uint64_t index = (uint64_t)addr / 4096;
    
    if (page_bitmap[index] == true) { return; }

    if (page_bitmap.set(index, true))
    {
        free_memory -= 4096;
        reserved_memory += 4096;
    }
}

void PageFrameAllocator::reserve_pages(void* addr, uint64_t page_count)
{
    for (int i = 0; i < page_count; i++)
    {
        reserve_page((void*)((uint64_t)addr + (i * 4096)));
    }
}

uint64_t PageFrameAllocator::get_free_mem()
{
    return free_memory;
}

uint64_t PageFrameAllocator::get_used_mem()
{
    return used_memory;
}

uint64_t PageFrameAllocator::get_reserved_mem()
{
    return reserved_memory;
}