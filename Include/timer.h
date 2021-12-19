#pragma once

#include <stdint.h>
#include <sys/time.h>
#include <time.h>

typedef struct Timer_
{
    struct timespec start, end;
} Timer;

void timerDelayMs(uint32_t ms);
void timerGetFPS(Timer *pTimer, uint32_t *fFPS);
