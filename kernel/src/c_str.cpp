#include "c_str.h"

char int_to_string_output[128];
const char* to_string(int64_t value)
{
    bool is_negative;

    if (value < 0)
    {
        is_negative = true;
        value *= -1;
        int_to_string_output[0] = '-';
    }

    uint8_t size;
    uint64_t size_probe = value;

    while (size_probe / 10 > 0) // probing how big the input is
    {
        size_probe /= 10;
        size++;
    }

    uint8_t index = 0;

    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        int_to_string_output[is_negative + size - index] = remainder + '0'; // setting char, 0 as start
        index++;
    }
    
    uint8_t remainder = value % 10;
    int_to_string_output[is_negative + size - index] = remainder + '0';
    int_to_string_output[is_negative + size + 1] = 0;

    return int_to_string_output;
}

char uint_to_string_output[128];
const char* to_string(uint64_t value)
{
    uint8_t size;
    uint64_t size_probe = value;

    while (size_probe / 10 > 0) // probing how big the input is
    {
        size_probe /= 10;
        size++;
    }

    uint8_t index = 0;

    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        uint_to_string_output[size - index] = remainder + '0'; // setting char, 0 as start
        index++;
    }
    
    uint8_t remainder = value % 10;
    uint_to_string_output[size - index] = remainder + '0';
    uint_to_string_output[size + 1] = 0;

    return uint_to_string_output;
}

char double_to_string_output[128];
const char* to_string(double value, uint8_t precision)
{
    if (precision > 20) { precision = 20; }
    
    char* int_ptr = (char*)to_string((int64_t)value);
    char* double_ptr = double_to_string_output;

    if (value < 0)
    {
        value *= -1;
    }

    while (*int_ptr != 0)
    {
        *double_ptr = *int_ptr;
        int_ptr++;
        double_ptr++;
    }

    *double_ptr = '.';
    double_ptr++;

    double new_value = value - (int)value;

    for (uint8_t i = 0; i < precision; i++)
    {
        new_value *= 10;
        *double_ptr = (int)new_value + '0';
        new_value -= (int)new_value;
        double_ptr++;
    }

    *double_ptr = 0;

    return double_to_string_output;
}

const char* to_string(double value)
{
    return to_string(value, 4);
}

char hex_to_string_output_64[128];
const char* to_hstring(uint64_t value)
{
    uint64_t* value_ptr = &value;
    uint8_t* tmp_ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;

    for (uint8_t i = 0; i < size; i++)
    {
        tmp_ptr = (uint8_t*)value_ptr + i;
        tmp = (*tmp_ptr & 0xF0) >> 4;
        hex_to_string_output_64[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = *tmp_ptr & 0x0F;
        hex_to_string_output_64[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hex_to_string_output_64[size + 1] = 0;

    return hex_to_string_output_64;
}

char hex_to_string_output_32[128];
const char* to_hstring(uint32_t value)
{
    uint32_t* value_ptr = &value;
    uint8_t* tmp_ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1;

    for (uint8_t i = 0; i < size; i++)
    {
        tmp_ptr = (uint8_t*)value_ptr + i;
        tmp = (*tmp_ptr & 0xF0) >> 4;
        hex_to_string_output_32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = *tmp_ptr & 0x0F;
        hex_to_string_output_32[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hex_to_string_output_32[size + 1] = 0;

    return hex_to_string_output_32;
}

char hex_to_string_output_16[128];
const char* to_hstring(uint16_t value)
{
    uint16_t* value_ptr = &value;
    uint8_t* tmp_ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1;

    for (uint8_t i = 0; i < size; i++)
    {
        tmp_ptr = (uint8_t*)value_ptr + i;
        tmp = (*tmp_ptr & 0xF0) >> 4;
        hex_to_string_output_16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = *tmp_ptr & 0x0F;
        hex_to_string_output_16[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hex_to_string_output_16[size + 1] = 0;

    return hex_to_string_output_16;
}

char hex_to_string_output_8[128];
const char* to_hstring(uint8_t value)
{
    uint8_t* value_ptr = &value;
    uint8_t* tmp_ptr;
    uint8_t tmp;
    uint8_t size = 1;

    for (uint8_t i = 0; i < size; i++)
    {
        tmp_ptr = (uint8_t*)value_ptr + i;
        tmp = (*tmp_ptr & 0xF0) >> 4;
        hex_to_string_output_8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = *tmp_ptr & 0x0F;
        hex_to_string_output_8[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hex_to_string_output_8[size + 1] = 0;

    return hex_to_string_output_8;
}