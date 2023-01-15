#include "interrupts.h"
#include "../panic_screen.h"
#include "../io.h"

__attribute__((interrupt)) void page_fault_handler(struct InterruptFrame* frame)
{
    panic("PAGE FAULT DETECTED");

    while (true);
}

__attribute__((interrupt)) void double_fault_handler(struct InterruptFrame* frame)
{
    panic("DOUBLE FAULT DETECTED");
    
    while (true);
}

__attribute__((interrupt)) void gp_fault_handler(struct InterruptFrame* frame)
{
    panic("GENERAL PROTECTION FAULT DETECTED");
    
    while (true);
}

__attribute__((interrupt)) void keyboard_int_handler(struct InterruptFrame* frame)
{
    global_renderer->print("PRESSED");

    uint8_t scancode = inb(0x60);
    pic_end_master();
}

void pic_end_master()
{
    outb(PIC1_CMD, PIC_EOI);
}

void pic_end_slave()
{
    outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}

void remap_pic() // remapping pic chip, because these values are used by exceptions
{
    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);
}