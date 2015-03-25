#include <unistd.h>
#include <bits/time.h>
#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include "times.h"

#define CLK sysconf(_SC_CLK_TCK)

void checkTimes(struct tms *prevTimes, clock_t *prevReal,
        struct tms *firstTimes, clock_t *firstReal) {

    struct tms now;
    times(&now);
    clock_t nowReal = clock();
    if (prevTimes->tms_stime == -1) {
        *firstTimes = now;
        *firstReal = nowReal;
    } else {
        printf("\n\tFrom the first check:\t\tR %.6f\tS %.6f\tU %.6f",
                ((double) (nowReal - *(firstReal))) / CLOCKS_PER_SEC,
                ((double) (now.tms_stime - firstTimes->tms_stime)) / CLK,
                ((double) (now.tms_utime - firstTimes->tms_utime)) / CLK);

        printf("\n\tFrom the previous check:\tR %.6f\tS %.6f\tU %.6f",
                ((double) (nowReal - *(prevReal))) / CLOCKS_PER_SEC,
                ((double) (now.tms_stime - prevTimes->tms_stime)) / CLK,
                ((double) (now.tms_utime - prevTimes->tms_utime)) / CLK);
    }
    printf("\n\tTime:\t\t\t\tR %.6f\tS %.6f\tU %.6f\n\n",
            ((double) nowReal) / CLOCKS_PER_SEC,
            ((double) now.tms_stime) / CLK,
            ((double) now.tms_utime) / CLK);
    *prevReal = nowReal;
    *prevTimes = now;
}
