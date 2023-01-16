#include "keyboard.h"

bool left_shift_pressed;
bool right_shift_pressed;

void handle_keyboard(uint8_t scancode)
{
    switch (scancode)
    {
        case LEFT_SHIFT:
            left_shift_pressed = true;
            return;
        case LEFT_SHIFT + 0x80:
            left_shift_pressed = false;
            return;
        case RIGHT_SHIFT:
            right_shift_pressed = true;
            return;
        case RIGHT_SHIFT + 0x80:
            right_shift_pressed = false;
            return;
        case ENTER:
            global_renderer->nextln();
            return;
        case SPACE:
            global_renderer->draw_char(' ');
            return;
        case BACKSPACE:
            global_renderer->clear_char();
            return;
    }

    char ascii = QWERTZKeyboard::translate(scancode, left_shift_pressed | right_shift_pressed);

    if (ascii != 0)
    {
        global_renderer->draw_char(ascii);
    }
}