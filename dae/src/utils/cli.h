
#ifndef __daex_cli_h__
#define __daex_cli_h__

#include <iomanip>

#include <eo>

#include "utils/evalBestPlanDump.h"
#include "core/decomposition.h"

#define LOG_FILL ' '
#define FORMAT_LEFT_FILL_WIDTH(width) "\t" << std::left << std::setfill(LOG_FILL) << std::setw(width) 
#define FORMAT_LEFT_FILL_W_PARAM FORMAT_LEFT_FILL_WIDTH(22)

void print_results( eoPop<daex::Decomposition> pop, time_t time_start, int run );

#endif

