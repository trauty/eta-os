#include "panic_screen.h"
#include "basic_renderer.h"
#include "c_str.h"

void panic(const char* msg)
{
    uint16_t msg_size;
    while (msg[msg_size] != '\0') { msg_size++; } msg_size++;

    global_renderer->clear(0x0000ff00);

    global_renderer->cursor_position = { global_renderer->target_framebuffer->width / 2 - 52, global_renderer->target_framebuffer->height / 2 - 16 };

    global_renderer->print("KERNEL PANIC");

    global_renderer->cursor_position = { global_renderer->target_framebuffer->width / 2 - (msg_size * 8) / 2, global_renderer->cursor_position.y + 32 };

    global_renderer->print(msg);
}