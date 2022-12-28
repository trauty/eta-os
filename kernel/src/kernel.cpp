#include <stddef.h>

#include "c_str.h"
#include "basic_renderer.h"

extern "C" void start(Framebuffer* framebuffer, PSF1_FONT* psf1_font)
{
    BasicRenderer new_renderer(framebuffer, psf1_font, 0xffffffff);
    new_renderer.print(to_string((uint64_t)213));
    new_renderer.cursor_position = { 0, 16 };
    new_renderer.print(to_string((int64_t)-123));
    new_renderer.cursor_position = { 0, 32 };
    new_renderer.print(to_string((double)-14.1566));
    new_renderer.cursor_position = { 0, 48 };
    new_renderer.print(to_hstring((uint64_t)0xF0));
    new_renderer.cursor_position = { 0, 64 };
    new_renderer.print(to_hstring((uint32_t)0xF0));
    new_renderer.cursor_position = { 0, 80 };
    new_renderer.print(to_hstring((uint16_t)0xF0));
    new_renderer.cursor_position = { 0, 96 };
    new_renderer.print(to_hstring((uint8_t)0xF0));


    new_renderer.cursor_position = { new_renderer.target_framebuffer->width / 2 + 50, new_renderer.target_framebuffer->height / 2 - 32 };
    new_renderer.print("DEEZ NUTS - PHILIP IST GAY");
    
    new_renderer.cursor_position = { new_renderer.target_framebuffer->width / 2 + 50, new_renderer.target_framebuffer->height / 2 };
    new_renderer.print(" ############");
    new_renderer.cursor_position = { new_renderer.target_framebuffer->width / 2 + 50, new_renderer.target_framebuffer->height / 2 + 16 };
    new_renderer.print("###############");
    new_renderer.cursor_position = { new_renderer.target_framebuffer->width / 2 + 50, new_renderer.target_framebuffer->height / 2 + 32 };
    new_renderer.print("###-----------##");
    new_renderer.cursor_position = { new_renderer.target_framebuffer->width / 2 + 50, new_renderer.target_framebuffer->height / 2 + 48 };
    new_renderer.print("################");
    new_renderer.cursor_position = { new_renderer.target_framebuffer->width / 2 + 50, new_renderer.target_framebuffer->height / 2 + 64 };
    new_renderer.print("################");
    new_renderer.cursor_position = { new_renderer.target_framebuffer->width / 2 + 50, new_renderer.target_framebuffer->height / 2 + 80 };
    new_renderer.print("################");
    new_renderer.cursor_position = { new_renderer.target_framebuffer->width / 2 + 50, new_renderer.target_framebuffer->height / 2 + 96 };
    new_renderer.print("################");
    new_renderer.cursor_position = { new_renderer.target_framebuffer->width / 2 + 50, new_renderer.target_framebuffer->height / 2 + 112 };
    new_renderer.print("###          ###");
    new_renderer.cursor_position = { new_renderer.target_framebuffer->width / 2 + 50, new_renderer.target_framebuffer->height / 2 + 128 };
    new_renderer.print("###          ###");


    return;
}