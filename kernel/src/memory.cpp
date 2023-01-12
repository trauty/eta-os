#include "memory.h"

uint64_t get_memory_size(EFI_MEMORY_DESCRIPTOR* mem_map, uint64_t mem_map_entries, uint64_t mem_map_desc_size)
{
    static uint64_t mem_size_bytes = 0;
    if (mem_size_bytes > 0) { return mem_size_bytes; }

    for (int i = 0; i < mem_map_entries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mem_map + (i * mem_map_desc_size));
        mem_size_bytes += desc->num_pages * 4096;
    }

    return mem_size_bytes;
}

void memset(void* start, uint8_t value, uint64_t num)
{
    for (uint64_t i = 0; i < num; i++)
    {
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}