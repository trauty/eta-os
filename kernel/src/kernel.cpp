#include <stddef.h>

#include "c_str.h"
#include "memory.h"
#include "bitmap.h"
#include "efi_memory.h"
#include "basic_renderer.h"
#include "page_frame_allocator.h"

struct BOOT_INFO
{
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
	void* mem_map;
	uint64_t mem_map_size;
	uint64_t mem_map_descriptor_size;
};

extern uint64_t _kernel_start;
extern uint64_t _kernel_end;

extern "C" void start(BOOT_INFO * boot_info)
{
    BasicRenderer new_renderer(boot_info->framebuffer, boot_info->psf1_font, 0xffffffff);
    uint64_t mem_map_entries = boot_info->mem_map_size / boot_info->mem_map_descriptor_size;

    PageFrameAllocator new_alloc;

    new_alloc.read_efi_memory_map((EFI_MEMORY_DESCRIPTOR*)boot_info->mem_map, boot_info->mem_map_size, boot_info->mem_map_descriptor_size);

    uint64_t kernel_size = (uint64_t)&_kernel_end - (uint64_t)&_kernel_start;
    uint64_t kernel_pages = (uint64_t)kernel_size / 4096 + 1;

    new_alloc.lock_pages(&_kernel_start, kernel_pages);
    
    new_renderer.print("Free RAM: ");
    new_renderer.print(to_string(new_alloc.get_free_mem() / 1024));
    new_renderer.print("kB");
    new_renderer.cursor_position = { 0, new_renderer.cursor_position.y + 16 };

    new_renderer.print("Used RAM: ");
    new_renderer.print(to_string(new_alloc.get_used_mem() / 1024));
    new_renderer.print("kB");
    new_renderer.cursor_position = { 0, new_renderer.cursor_position.y + 16 };

    new_renderer.print("Reserved RAM: ");
    new_renderer.print(to_string(new_alloc.get_reserved_mem() / 1024));
    new_renderer.print("kB");
    new_renderer.cursor_position = { 0, new_renderer.cursor_position.y + 16 };

    for (int i = 0; i < 20; i++)
    {
        void* crt_addr = new_alloc.request_page();
        new_renderer.print(to_hstring((uint64_t)crt_addr));
        new_renderer.cursor_position = { 0, new_renderer.cursor_position.y + 16 };
    }

    // for (int i = 0; i < mem_map_entries; i++)
    // {
    //     EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)boot_info->mem_map + (i * boot_info->mem_map_descriptor_size));
    //     new_renderer.print(EFI_MEMORY_TYPE_STRINGS[desc->type]);
    //     new_renderer.print(": ");
    //     new_renderer.color = 0x00ffff;
    //     new_renderer.print(to_string(desc->num_pages * 4096 / 1024));
    //     new_renderer.print("kB");
    //     new_renderer.color = 0xffffffff;
    //     new_renderer.cursor_position = { 0, new_renderer.cursor_position.y + 16 };
    // }

    return;
}