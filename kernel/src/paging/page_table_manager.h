#pragma once

#include "paging.h"

class PageTableManager
{
public:
    PageTableManager(PageTable* pml4_addr);
    PageTable* pml4_addr;
    void map_mem(void* virt_mem, void* phys_mem);
};