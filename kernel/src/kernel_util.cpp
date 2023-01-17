#include "kernel_util.h"

KernelInfo kernel_info;

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

    global_ptm = PageTableManager(pml4);

    for (uint64_t i = 0; i < get_memory_size((EFI_MEMORY_DESCRIPTOR*)boot_info->mem_map, mem_map_entries, boot_info->mem_map_descriptor_size); i += 4096)
    {
        global_ptm.map_mem((void*)i, (void*)i);
    }

    uint64_t frame_buf_base = (uint64_t)boot_info->framebuffer->base_adress;
    uint64_t frame_buf_size = (uint64_t)boot_info->framebuffer->buffer_size + 4096;

    global_allocator.lock_pages((void*)frame_buf_base, frame_buf_size / 4096 + 1);
        
    for (uint64_t i = frame_buf_base; i < frame_buf_base + frame_buf_size; i += 4096)
    {
        global_ptm.map_mem((void*)i, (void*)i);
    }

    asm("mov %0, %%cr3" : : "r" (pml4)); // mmu will now use page tables

    kernel_info.page_table_mgr = &global_ptm;
}

IDTR idtr;

void set_idt_gate(void* handler, uint8_t entry_offset, uint8_t type_attr, uint8_t selector)
{
    IDTDescEntry* interrupt = (IDTDescEntry*)(idtr.offset + entry_offset * sizeof(IDTDescEntry));
    interrupt->set_offset((uint64_t)handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
}

void prepare_interrupts()
{
    idtr.limit = 0x0fff;
    idtr.offset = (uint64_t)global_allocator.request_page();

    set_idt_gate((void*)page_fault_handler, 0xe, IDT_TA_INTERRUPTGATE, 0x08);
    set_idt_gate((void*)double_fault_handler, 0x8, IDT_TA_INTERRUPTGATE, 0x08);
    set_idt_gate((void*)gp_fault_handler, 0xd, IDT_TA_INTERRUPTGATE, 0x08);
    set_idt_gate((void*)keyboard_int_handler, 0x21, IDT_TA_INTERRUPTGATE, 0x08);
    set_idt_gate((void*)pit_int_handler, 0x20, IDT_TA_INTERRUPTGATE, 0x08);

    asm("lidt %0" : : "m" (idtr)); // moving value into %0 and calling handler

    remap_pic();
}

void prepare_acpi(BOOT_INFO* boot_info)
{
    ACPI::SDTHeader* xsdt = (ACPI::SDTHeader*)(boot_info->rsdp->xsdt_adrr);

    ACPI::MCFGHeader* mcfg = (ACPI::MCFGHeader*)ACPI::find_table(xsdt, (char*)"MCFG");

    PCI::enumerate_pci(mcfg);
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

    init_heap((void*)0x0000100000000000, 16);
    
    prepare_interrupts();

    prepare_acpi(boot_info);

    outb(PIC1_DATA, 0b11111000); // unmasking
    outb(PIC2_DATA, 0b11101111); // unmasking

    asm("sti"); // set interrupt flag to react to maskable interrupts

    return kernel_info;
}