/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <metal/time.h>
#include <metal/timer.h>

#include <stddef.h>

int metal_gettimeofday(struct timeval *tp, void *tzp) {
    int rv;
    unsigned long long mcc, timebase;
    rv = metal_timer_get_cyclecount(0, &mcc);
    if (rv != 0) {
        return -1;
    }
    rv = metal_timer_get_timebase_frequency(0, &timebase);
    if (rv != 0) {
        return -1;
    }
    tp->tv_sec = mcc / timebase;
    tp->tv_usec = mcc % timebase * 1000000 / timebase;
    return 0;
}

time_t metal_time(void) {
    struct timeval now;

    if (metal_gettimeofday(&now, NULL) < 0)
        now.tv_sec = (time_t)-1;

    return now.tv_sec;
}
