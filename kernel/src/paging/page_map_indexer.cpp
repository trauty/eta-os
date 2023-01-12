#include "page_map_indexer.h"

PageMapIndexer::PageMapIndexer(uint64_t virt_addr) // bitmasking till all are aligned
{
    virt_addr >>= 12;
    p_i = virt_addr & 0x1ff; 
    virt_addr >>= 9;
    pt_i = virt_addr & 0x1ff; 
    virt_addr >>= 9;
    pd_i = virt_addr & 0x1ff; 
    virt_addr >>= 9;
    pdp_i = virt_addr & 0x1ff;
}