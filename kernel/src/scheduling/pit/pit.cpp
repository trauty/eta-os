#include "pit.h"

namespace PIT
{
    double time_since_boot = 0;

    uint16_t divisor = 65535;

    void sleepd(double seconds)
    {
        double start_time = time_since_boot;

        while (time_since_boot < start_time + seconds)
        {
            asm("hlt");
        }
    }

    void sleep(uint64_t ms)
    {
        sleepd((double)ms / 1000);
    }

    void set_divisor(uint16_t divisor)
    {
        if (divisor < 100) { divisor = 100; }
        divisor = divisor;
        outb(0x40, (uint8_t)(divisor & 0x00ff));
        io_wait();
        outb(0x40, ((uint8_t)(divisor & 0xff00) >> 8));
    }

    uint64_t get_freq() { return base_freq / divisor; }

    void set_freq(uint64_t freq) { set_divisor(base_freq / freq); }

    void tick() { time_since_boot += 1 / (double)get_freq(); }
}