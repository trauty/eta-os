#include "gdt.h"

__attribute__((aligned(4096)))
GDT default_gdt =
{
    {0, 0, 0, 0x00, 0x00, 0}, // kernel null segment
    {0, 0, 0, 0x9a, 0xa0, 0}, // kernel code segment
    {0, 0, 0, 0x92, 0xa0, 0}, // kernel data segment
    {0, 0, 0, 0x00, 0x00, 0}, // user null segment
    {0, 0, 0, 0x9a, 0xa0, 0}, // user code segment
    {0, 0, 0, 0x92, 0xa0, 0}  // user data segment    
};