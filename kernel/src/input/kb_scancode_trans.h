#pragma once

#include <stdint.h>

namespace QWERTZKeyboard
{
    #define LEFT_SHIFT 0x2a
    #define RIGHT_SHIFT 0x36
    #define ENTER 0x1c
    #define BACKSPACE 0x0e
    #define SPACE 0x39
    
    extern const char ascii_table[];
    char translate(uint8_t scancode, bool uppercase);
}
