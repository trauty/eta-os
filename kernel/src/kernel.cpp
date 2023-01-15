#include "kernel_util.h"

extern "C" void start(BOOT_INFO* boot_info)
{
    KernelInfo kernel_info = init_kernel(boot_info);
    PageTableManager* page_table_mgr = kernel_info.page_table_mgr;

    global_renderer->print("Eta-OS Kernel successfully initialized.");

    while (true);
}