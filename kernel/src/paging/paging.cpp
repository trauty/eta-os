#include "paging.h"

void PageDirEntry::set_flag(PT_FLAG flag, bool enabled)
{
    uint64_t bit_selector = (uint64_t)1 << flag;
    value &= ~bit_selector;

    if (enabled)
    {
        value |= bit_selector;
    }
}

bool PageDirEntry::get_flag(PT_FLAG flag)
{
    uint64_t bit_selector = (uint64_t)1 << flag;
    return value & bit_selector > 0 ? true : false;
}

uint64_t PageDirEntry::get_address()
{
    return (value & 0x000ffffffffff000) >> 12;
}

void PageDirEntry::set_address(uint64_t addr)
{
    addr &= 0x000000ffffffffff;
    value &= 0xfff0000000000fff;
    value |= (addr << 12);
}