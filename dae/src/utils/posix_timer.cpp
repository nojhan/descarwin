
#include "posix_timer.h"

PosixTimer::PosixTimer()
{
    times( & _start );
}


void PosixTimer::restart()
{
    times( & _start );
}


double PosixTimer::elapsed()
{
    times( & _end );

    return static_cast<double>( _end.tms_utime - _start.tms_utime ) / static_cast<double>( sysconf(_SC_CLK_TCK) );
}

