
#include "cli.h"

void print_results( eoPop<daex::Decomposition> pop, time_t time_start, int run )
{
    (void) time_start;
#ifndef NDEBUG
    struct rusage usage;
    getrusage(RUSAGE_SELF,&usage);

    eo::log << eo::warnings << "OS statistics:" << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "user time used"               << usage.ru_utime.tv_sec << "." << usage.ru_utime.tv_usec << " s." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "system time used"             << usage.ru_stime.tv_sec << "." << usage.ru_stime.tv_usec << " s." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "page reclaims * page size"   << usage.ru_minflt * getpagesize() << " b." 
                                         << " (~ " << std::setprecision(2) << usage.ru_minflt * getpagesize() / 1048576 << " Mb.)" << std::endl; 
    // 1 giga-byte = 1 073 741 824 bytes
    // 1 mega-byte = 1 048 576 octets 
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "page reclaims"                << usage.ru_minflt               << std::endl;
    eo::log << eo::warnings << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "page faults"                  << usage.ru_majflt               << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "maximum resident set size"    << usage.ru_maxrss               << " b." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "integral shared memory size"  << usage.ru_ixrss                << " b." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "integral unshared data size"  << usage.ru_idrss                << " b." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "integral unshared stack size" << usage.ru_isrss                << " b." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "swaps"                        << usage.ru_nswap                << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "block input operations"       << usage.ru_inblock              << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "block output operations"      << usage.ru_oublock              << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "messages sent"                << usage.ru_msgsnd               << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "messages received"            << usage.ru_msgrcv               << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "signals received"             << usage.ru_nsignals             << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "voluntary context switches"   << usage.ru_nvcsw                << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "involuntary context switches" << usage.ru_nivcsw               << std::endl;
    
    //double subsolver_time = 0;
    unsigned int subsolver_steps = 0;
    for( unsigned int i=0; i < pop.size(); i++ ) {
        for( unsigned int j=0; j < pop[i].subplans().size(); j++ ) {
             //subsolver_time += pop[i].subplan(j).time_subsolver();
             subsolver_steps += pop[i].subplan(j).search_steps();
        }
    }

    eo::log << eo::progress << "DAEx sub-solver steps " << subsolver_steps << std::endl;
    //eo::log << eo::progress << "; DAEx sub-solver time " << subsolver_time << " s (u-CPU)" << std::endl;
    eo::log << eo::progress << "DAEx user time " << usage.ru_utime.tv_sec << "." << usage.ru_utime.tv_usec << " (seconds of user time in CPU)" << std::endl;
    eo::log << eo::progress << "DAEx wallclock time " << std::difftime( std::time(NULL), time_start )  << " (seconds)" << std::endl;
#endif

    std::cout << "Number of restarts " << run << std::endl;

    // the pop being unsorted, sorting it before getting the first is more efficient
    // than using best_element (that uses std::max_element)
    //pop.sort();

#ifndef NDEBUG
    //std::cout << IPC_PLAN_COMMENT << pop.front() << std::endl;
    std::cout << IPC_PLAN_COMMENT << pop.best_element() << std::endl;
#endif
    //std::cout << pop.front().plan() << std::endl;
    std::cout << pop.best_element().plan_copy() << std::endl;
}

