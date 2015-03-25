#ifndef _TIMES_H_
#define _TIMES_H_

void checkTimes(struct tms *prevTimes, clock_t *prevReal,
        struct tms *firstTimes, clock_t *firstReal);

#endif //_TIMES_H_
