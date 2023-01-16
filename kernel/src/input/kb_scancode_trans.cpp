#include "kb_scancode_trans.h"

namespace QWERTZKeyboard
{
    const char ascii_table[] =
    {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8',
        '9', '0', '-', '=', 0, 0, 'q', 'w', 'e',
        'r', 't', 'z', 'u', 'i', 'o', 'p', '[', ']',
        0, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k',
        'l', ';', '\'', '`', 0, '\\', 'y', 'x', 'c',
        'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
    };

    char translate(uint8_t scancode, bool uppercase)
    {
        if (scancode > 58) { return 0; }

        if (uppercase)
        {
            return ascii_table[scancode] - 32;
        }
        else
        {
            return ascii_table[scancode];   
        }
    }
}