#include "kernel_util.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"

extern "C" void start(BOOT_INFO* boot_info)
{
    KernelInfo kernel_info = init_kernel(boot_info);
    PageTableManager* page_table_mgr = kernel_info.page_table_mgr;

    PIT::set_divisor(2048);
    
    global_renderer->print("Eta-OS Kernel successfully initialized.");
    global_renderer->nextln();
    global_renderer->print(to_string((uint64_t)boot_info->rsdp));
    global_renderer->nextln();

    global_renderer->print(to_hstring((uint64_t)malloc(0x8000)));
    global_renderer->nextln();
    void* addr = malloc(0x8000);
    global_renderer->print(to_hstring((uint64_t)addr));
    global_renderer->nextln();
    global_renderer->print(to_hstring((uint64_t)malloc(0x100)));
    global_renderer->nextln();

    free(addr);

    global_renderer->print(to_hstring((uint64_t)malloc(0x100)));
    global_renderer->nextln();

    for (int i = 0; i < 20; i++)
    {
        global_renderer->print("g");
        PIT::sleep(100);
    }

    while (true);
}