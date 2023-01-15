#include "io.h"
#include "gdt/gdt.h"
#include "kernel_util.h"
#include "interrupts/idt.h"
#include "interrupts/interrupts.h"

KernelInfo kernel_info;
PageTableManager page_table_mgr = NULL;

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

IDTR idtr;

void prepare_interrupts()
{
    idtr.limit = 0x0fff;
    idtr.offset = (uint64_t)global_allocator.request_page();

    IDTDescEntry* int_page_fault = (IDTDescEntry*)(idtr.offset + 0xe * sizeof(IDTDescEntry));
    int_page_fault->set_offset((uint64_t)page_fault_handler);
    int_page_fault->type_attr = IDT_TA_INTERRUPTGATE;
    int_page_fault->selector = 0x08;

    IDTDescEntry* int_double_fault = (IDTDescEntry*)(idtr.offset + 0x8 * sizeof(IDTDescEntry));
    int_double_fault->set_offset((uint64_t)double_fault_handler);
    int_double_fault->type_attr = IDT_TA_INTERRUPTGATE;
    int_double_fault->selector = 0x08;

    IDTDescEntry* int_gp_fault = (IDTDescEntry*)(idtr.offset + 0xd * sizeof(IDTDescEntry));
    int_gp_fault->set_offset((uint64_t)gp_fault_handler);
    int_gp_fault->type_attr = IDT_TA_INTERRUPTGATE;
    int_gp_fault->selector = 0x08;

    IDTDescEntry* int_keyboard = (IDTDescEntry*)(idtr.offset + 0x21 * sizeof(IDTDescEntry));
    int_keyboard->set_offset((uint64_t)keyboard_int_handler);
    int_keyboard->type_attr = IDT_TA_INTERRUPTGATE;
    int_keyboard->selector = 0x08;

    asm("lidt %0" : : "m" (idtr)); // moving value into %0 and calling handler

    remap_pic();

    outb(PIC1_DATA, 0b11111101); // unmask second bit
    outb(PIC2_DATA, 0b11111111); // mask all bits of second pic

    asm("sti"); // to cancel masked interrupts: asm("cli");
}

BasicRenderer r = BasicRenderer(NULL, NULL, 0xffffffff);

KernelInfo init_kernel(BOOT_INFO* boot_info)
{
    r = BasicRenderer(boot_info->framebuffer, boot_info->psf1_font, 0xffffffff);
    global_renderer = &r;

    GDTDescriptor gdt_desc;
    gdt_desc.size = sizeof(GDT) - 1;
    gdt_desc.offset = (uint64_t)&default_gdt;
    load_gdt(&gdt_desc);
    
    prepare_mem(boot_info);

    memset(boot_info->framebuffer->base_adress, 0, boot_info->framebuffer->buffer_size);

    prepare_interrupts();

    return kernel_info;
}