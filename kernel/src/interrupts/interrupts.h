#pragma once

#include "../basic_renderer.h"
#include "../panic_screen.h"
#include "../io.h"
#include "../input/keyboard.h"
#include "../scheduling/pit/pit.h"

#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct InterruptFrame;
__attribute__((interrupt)) void page_fault_handler(InterruptFrame* frame);
__attribute__((interrupt)) void double_fault_handler(InterruptFrame* frame);
__attribute__((interrupt)) void gp_fault_handler(InterruptFrame* frame);
__attribute__((interrupt)) void keyboard_int_handler(InterruptFrame* frame);
__attribute__((interrupt)) void pit_int_handler(InterruptFrame* frame);

void remap_pic();
void pic_end_master();
void pic_end_slave();