#pragma once

#include <stdint.h>

#include "math_util.h"
#include "framebuffer.h"
#include "simple_font.h"

class BasicRenderer
{
public:
    BasicRenderer(Framebuffer* target_framebuffer, PSF1_FONT* psf1_font, unsigned int color)
    {
        this->target_framebuffer = target_framebuffer;
        this->psf1_font = psf1_font;
        this->color = color;
        this->cursor_position = { 0,0 };
    };

    Point cursor_position;
    Framebuffer* target_framebuffer;
    PSF1_FONT* psf1_font;
    unsigned int color;
    unsigned int clear_color;
    void print(const char* str);
    void draw_char(char chr, unsigned int xOff, unsigned yOff);
    void draw_char(char chr);
    void clear();
    void clear_char();
    void nextln();
};

extern BasicRenderer* global_renderer;