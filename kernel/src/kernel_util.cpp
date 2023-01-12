#include "kernel_util.h"

KernelInfo kernel_info;
PageTableManager page_table_mgr = nullptr;

void prepare_mem(BOOT_INFO* boot_info)
{
    uint64_t mem_map_entries = boot_info->mem_map_size / boot_info->mem_map_descriptor_size;

    global_allocator = PageFrameAllocator();
    global_allocator.read_efi_memory_map((EFI_MEMORY_DESCRIPTOR*)boot_info->mem_map, boot_info->mem_map_size, boot_info->mem_map_descriptor_size);

    uint64_t kernel_size = (uint64_t)&_kernel_end - (uint64_t)&_kernel_start;
    uint64_t kernel_pages = (uint64_t)kernel_size / 4096 + 1;

    global_allocator.lock_pages(&_kernel_start, kernel_pages);

    PageTable* pml4 = (PageTable*)global_allocator.request_page();
    memset(pml4, 0, 4096);

    page_table_mgr = PageTableManager(pml4);

    for (uint64_t i = 0; i < get_memory_size((EFI_MEMORY_DESCRIPTOR*)boot_info->mem_map, mem_map_entries, boot_info->mem_map_descriptor_size); i += 4096)
    {
        page_table_mgr.map_mem((void*)i, (void*)i);
    }

    uint64_t frame_buf_base = (uint64_t)boot_info->framebuffer->base_adress;
    uint64_t frame_buf_size = (uint64_t)boot_info->framebuffer->buffer_size + 4096;

    global_allocator.lock_pages((void*)frame_buf_base, frame_buf_size / 4096 + 1);
        
    for (uint64_t i = frame_buf_base; i < frame_buf_base + frame_buf_size; i += 4096)
    {
        page_table_mgr.map_mem((void*)i, (void*)i);
    }

    asm("mov %0, %%cr3" : : "r" (pml4)); // mmu will now use page tables

    kernel_info.page_table_mgr = &page_table_mgr;
}

KernelInfo init_kernel(BOOT_INFO* boot_info)
{
    prepare_mem(boot_info);

    memset(boot_info->framebuffer->base_adress, 0, boot_info->framebuffer->buffer_size);

    return kernel_info;
}