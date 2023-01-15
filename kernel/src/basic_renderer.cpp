#include "basic_renderer.h"

BasicRenderer* global_renderer;

void BasicRenderer::draw_char(char chr, unsigned int xOff, unsigned yOff)
{
    unsigned int* pixel_ptr = (unsigned int*)target_framebuffer->base_adress;
    char* font_ptr = (char*)psf1_font->glyph_buffer + (chr * psf1_font->psf1_header->charsize);

    for (unsigned long y = yOff; y < yOff + 16; y++)
    {
        for (unsigned long x = xOff; x < xOff + 8; x++)
        {
            if ((*font_ptr & (0b10000000) >> (x - xOff)) > 0) // selecting bit in bitmap by bitshifting
            {
                *(unsigned int*)(pixel_ptr + x + (y * target_framebuffer->pixels_per_scanline)) = color;
            }
        }

        font_ptr++;
    }
}

void BasicRenderer::print(const char* str)
{
    char* chr = (char*)str;

    while (*chr != 0)
    {
        draw_char(*chr, cursor_position.x, cursor_position.y);
        cursor_position.x += 8;

        if (cursor_position.x + 8 > target_framebuffer->width)
        {
            cursor_position.x = 0;
            cursor_position.y += 16;
        }

        chr++;
    }
}

void BasicRenderer::clear(uint32_t color)
{
    uint64_t frame_buf_base = (uint64_t)target_framebuffer->base_adress;
    uint64_t bytes_per_scanline = target_framebuffer->pixels_per_scanline * 4; // each pixel 4 bytes
    uint64_t frame_buf_height = target_framebuffer->height;
    uint64_t frame_buf_size = target_framebuffer->buffer_size;

    for (int vert_scan_line = 0; vert_scan_line < frame_buf_height; vert_scan_line++)
    {
        uint64_t pix_ptr_base = frame_buf_base + (bytes_per_scanline * vert_scan_line);
        for (uint32_t* pix_ptr = (uint32_t*)pix_ptr_base; pix_ptr < (uint32_t*)(pix_ptr_base + bytes_per_scanline); pix_ptr++)
        {
            *pix_ptr = color;
        }
    }
}

void BasicRenderer::nextln()
{
    cursor_position.x = 0;
    cursor_position.y += 16;
}