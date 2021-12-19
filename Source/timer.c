#include "timer.h"

void timerDelayMs(uint32_t ms)
{
    uint64_t delta_ms = 0;
    Timer delay;
    clock_gettime(CLOCK_MONOTONIC_RAW, &delay.start);
    while (delta_ms < ms)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &delay.end);
        delta_ms = (delay.end.tv_sec - delay.start.tv_sec) * 1000 +
                   (delay.end.tv_nsec - delay.start.tv_nsec) / 1000000;
    }
}

void timerGetFPS(Timer *pTimer, uint32_t *fFPS)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &pTimer->end);
    long fps = 1000000000 / (pTimer->end.tv_nsec - pTimer->start.tv_nsec);
    pTimer->start = pTimer->end;
    *fFPS = fps;
}
