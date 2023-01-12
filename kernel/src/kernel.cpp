#include "kernel_util.h"

extern "C" void start(BOOT_INFO * boot_info)
{
    KernelInfo kernel_info = init_kernel(boot_info);
    PageTableManager* page_table_mgr = kernel_info.page_table_mgr;

    BasicRenderer new_renderer(boot_info->framebuffer, boot_info->psf1_font, 0xffffffff);

    new_renderer.print("Eta-OS Kernel successfully initialized.");

    for (int i = 0; i < 2000; i++)
    {
        global_allocator.request_page();
    }

    while (true);
}