#pragma once

#include <stdint.h>

#include "../../io.h"

namespace PIT
{
    extern double time_since_boot;
    const uint64_t base_freq = 1193182;

    void sleepd(double seconds);
    void sleep(uint64_t ms);

    void set_divisor(uint16_t divisor);
    uint64_t get_freq();
    void set_freq(uint64_t freq);
    void tick();
}