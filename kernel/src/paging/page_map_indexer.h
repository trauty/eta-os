#pragma once

#include <stdint.h>

class PageMapIndexer
{
public:
    PageMapIndexer(uint64_t virt_addr);
    uint64_t pdp_i; // Page Directory Pointer Index
    uint64_t pd_i;  // Page Directory Index
    uint64_t pt_i;  // Page Table Index
    uint64_t p_i;   // Page Index
};