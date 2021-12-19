#include "stats.h"

#include "stdint.h"
#include "stdio.h"

void StatsReset(Stats *pStats)
{
    pStats->min = INT64_MAX;
    pStats->max = INT64_MIN;
    pStats->avg = 0;
    pStats->avg_sum = 0;
    pStats->collectionIt = 0;
}

void StatsCollect(int64_t value, Stats *pStats)
{
    pStats->collectionIt++;
    pStats->avg_sum += value;
    if (value < pStats->min)
        pStats->min = value;
    if (value > pStats->max)
        pStats->max = value;
}

void StatsRecord(Stats *pNew, Stats *pOld)
{
    if (pNew->collectionIt)
    {
        pNew->avg = pNew->avg_sum / pNew->collectionIt;
    }
    *pOld = *pNew;
    StatsReset(pNew);
}

int64_t StatsGetMax(Stats stats)
{
    return stats.max;
}

int64_t StatsGetMin(Stats stats)
{
    return stats.min;
}

int64_t StatsGetAvg(Stats stats)
{
    return stats.avg;
}

void StatsToString(Stats stats, char *pStatName, char *pString)
{
    sprintf(pString, "%s: max=%lld, min=%lld, avg=%lld",
            pStatName, stats.max, stats.min, stats.avg);
}
