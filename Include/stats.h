#pragma once

#include <stdint.h>

typedef struct Stats_
{
    int64_t min;
    int64_t max;
    int64_t avg;

    int64_t avg_sum;
    uint64_t collectionIt;
} Stats;

extern const Stats statsDefault;

void StatsReset(Stats *pStats);
void StatsCollect(int64_t value, Stats *pStats);
void StatsRecord(Stats *pNew, Stats *pOld);
int64_t StatsGetMax(Stats stats);
int64_t StatsGetMin(Stats stats);
int64_t StatsGetAvg(Stats stats);
void StatsToString(Stats stats, char *pStatName, char *pString);
