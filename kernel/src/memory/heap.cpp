#include "heap.h"

void* heap_start;
void* heap_end;
HeapSegHeader* last_header;

void init_heap(void* heap_addr, size_t page_count)
{
    void* pos = heap_addr;

    for (size_t i = 0; i < page_count; i++)
    {
        global_ptm.map_mem(pos, global_allocator.request_page());

        pos = (void*)((size_t)pos + 4096);
    }

    size_t heap_length = page_count * 4096;

    heap_start = heap_addr;
    heap_end = (void*)((size_t)heap_start + heap_length);
    HeapSegHeader* start_seg = (HeapSegHeader*)heap_addr;
    start_seg->length = heap_length - sizeof(HeapSegHeader);
    start_seg->next = NULL;
    start_seg->last = NULL;
    start_seg->free = true;
    last_header = start_seg;
}

void free(void* addr)
{
    HeapSegHeader* seg = (HeapSegHeader*)addr - 1;
    seg->free = true;
    seg->combine_forward();
    seg->combine_backward();
}

void* malloc(size_t size)
{
    if (size % 0x10 > 0)
    {
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0) { return NULL; }

    HeapSegHeader* cur_segment = (HeapSegHeader*)heap_start;

    while (true)
    {
        if (cur_segment->free)
        {
            if (cur_segment->length > size)
            {
                cur_segment->split(size);
                cur_segment->free = false;
                return (void*)((uint64_t)cur_segment + sizeof(HeapSegHeader));
            }
            
            if (cur_segment->length == size)
            {
                cur_segment->length == false;
                return (void*)((uint64_t)cur_segment + sizeof(HeapSegHeader));
            }
        }

        if (cur_segment->next == NULL) { break; }

        cur_segment = cur_segment->next;
    }

    extend_heap(size);

    return malloc(size);
}

HeapSegHeader* HeapSegHeader::split(size_t split_length)
{
    if (split_length < 0x10) { return NULL; }
    int64_t split_seg_length = length - split_length - (sizeof(HeapSegHeader));
    if (split_seg_length < 0x10) { return NULL; }

    HeapSegHeader* new_split_header = (HeapSegHeader*)((size_t)this + split_length + sizeof(HeapSegHeader));
    next->last = new_split_header;                  // next seg last seg to new seg
    new_split_header->next = next;                  // new seg next seg to original seg
    next = new_split_header;                        // new seg to new seg
    new_split_header->last = this;                  // new segs last seg to cur seg
    new_split_header->length = split_seg_length;    // new header's length to calculated value
    new_split_header->free = free;                  // new seg should be free
    length = split_length;                          //length of original seg to new length

    if (last_header == this)
    {
        last_header = new_split_header;
    }

    return new_split_header;
}

void extend_heap(size_t length)
{
    if (length % 0x1000)
    {
        length -= length % 0x1000;
        length += 0x1000;
    }

    size_t page_count = length / 0x1000;
    HeapSegHeader* new_segment = (HeapSegHeader*)heap_end;

    for (size_t i = 0; i < page_count; i++)
    {
        global_ptm.map_mem(heap_end, global_allocator.request_page());
        heap_end = (void*)((size_t)heap_end + 0x1000);
    }

    new_segment->free = true;
    new_segment->last = last_header;
    last_header->next = new_segment;
    last_header = new_segment;
    new_segment->next = NULL;
    new_segment->length = length - sizeof(HeapSegHeader);
    new_segment->combine_backward();
}

void HeapSegHeader::combine_forward()
{
    if (next == NULL) { return; }
    if (!next->free) { return; }

    if (next == last_header) { last_header = this; }
    
    if (next->next != NULL)
    {
        next->next->last = this;
    }
    
    length = length + next->length + sizeof(HeapSegHeader);
}

void HeapSegHeader::combine_backward()
{
    if (last != NULL && last->free) { last->combine_forward(); }
}