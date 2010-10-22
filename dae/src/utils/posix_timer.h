
#ifndef __POSIX_TIMER_H__
#define __POSIX_TIMER_H__

#include <sys/times.h>
#include <unistd.h>

class PosixTimer
{
public:
    PosixTimer(); 

    void restart();

    double elapsed();

    double get() { return elapsed(); }

protected:
    struct tms _start;
    struct tms _end;
};

#endif // __POSIX_TIMER_H__

