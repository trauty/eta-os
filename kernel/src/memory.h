#pragma once

#include <stdint.h>
#include "efi_memory.h"

uint64_t get_memory_size(EFI_MEMORY_DESCRIPTOR* mem_map, uint64_t mem_map_entries, uint64_t mem_map_desc_size);
void memset(void* start, uint8_t value, uint64_t num);