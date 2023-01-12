#pragma once

#include <stdint.h>

enum PT_FLAG
{
    Present = 0,
    ReadWrite = 1,
    UserSuper = 2,
    WriteThrough = 3,
    CacheDisabled = 4,
    Accessed = 5,
    LargerPages = 7,
    Custom0 = 9,
    Custom1 = 10,
    Custom2 = 11,
    NX = 63 // EXPERIMENTAL !! ONLY ON MODERN SYSTEMS
};

struct PageDirEntry
{
    uint64_t value;
    void set_flag(PT_FLAG flag, bool enabled);
    bool get_flag(PT_FLAG flag);
    void set_address(uint64_t addr);
    uint64_t get_address();
};

struct PageTable
{
    PageDirEntry entries[512];
    
}__attribute__((aligned(4096)));
