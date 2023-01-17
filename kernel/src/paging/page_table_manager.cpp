#include <stdint.h>

#include "../memory.h"
#include "page_map_indexer.h"
#include "page_table_manager.h"
#include "page_frame_allocator.h"

PageTableManager global_ptm = NULL;

PageTableManager::PageTableManager(PageTable* pml4_addr)
{
    this->pml4_addr = pml4_addr;
}

void PageTableManager::map_mem(void* virt_mem, void* phys_mem)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virt_mem);
    PageDirEntry pde;

    pde = pml4_addr->entries[indexer.pdp_i];
    PageTable* pdp;
    
    if (!pde.get_flag(PT_FLAG::Present))
    {
        pdp = (PageTable*)global_allocator.request_page();
        memset(pdp, 0, 4096);
        pde.set_address((uint64_t)pdp >> 12);
        pde.set_flag(PT_FLAG::Present, true);
        pde.set_flag(PT_FLAG::ReadWrite, true);
        pml4_addr->entries[indexer.pdp_i] = pde;
    }
    else
    {
        pdp = (PageTable*)((uint64_t)pde.get_address() << 12);
    }

    pde = pdp->entries[indexer.pd_i];
    PageTable* pd;
    
    if (!pde.get_flag(PT_FLAG::Present))
    {
        pd = (PageTable*)global_allocator.request_page();
        memset(pd, 0, 4096);
        pde.set_address((uint64_t)pd >> 12);
        pde.set_flag(PT_FLAG::Present, true);
        pde.set_flag(PT_FLAG::ReadWrite, true);
        pdp->entries[indexer.pd_i] = pde;
    }
    else
    {
        pd = (PageTable*)((uint64_t)pde.get_address() << 12);
    }

    pde = pd->entries[indexer.pt_i];
    PageTable* pt;
    
    if (!pde.get_flag(PT_FLAG::Present))
    {
        pt = (PageTable*)global_allocator.request_page();
        memset(pt, 0, 4096);
        pde.set_address((uint64_t)pt >> 12);
        pde.set_flag(PT_FLAG::Present, true);
        pde.set_flag(PT_FLAG::ReadWrite, true);
        pd->entries[indexer.pt_i] = pde;
    }
    else
    {
        pt = (PageTable*)((uint64_t)pde.get_address() << 12);
    }

    pde = pt->entries[indexer.p_i];
    pde.set_address((uint64_t)phys_mem >> 12);
    pde.set_flag(PT_FLAG::Present, true);
    pde.set_flag(PT_FLAG::ReadWrite, true);
    pt->entries[indexer.p_i] = pde;
}