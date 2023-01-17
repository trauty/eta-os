#pragma once

#include <stdint.h>
#include <stddef.h>

#include "../paging/page_table_manager.h"
#include "../paging/page_frame_allocator.h"

struct HeapSegHeader
{
    size_t length;
    HeapSegHeader* next;
    HeapSegHeader* last;
    bool free;
    
    void combine_forward();
    void combine_backward();
    HeapSegHeader* split(size_t split_length);
};

void init_heap(void* heap_addr, size_t page_count);

void* malloc(size_t size);
void free(void* addr);

void extend_heap(size_t length);