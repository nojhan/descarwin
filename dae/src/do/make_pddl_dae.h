/*
* <make_pddl_dae.h>
* Copyright (C) TAO Project-Team, INRIA Saclay-LRI && Thales Group, 2011-2012
* DESCARWIN ANR project 
* Author: <Mostepha R. Khouadjia>
* Copyright: See COPYING file that comes with this distribution
*/
 
#ifndef MAKE_PDDL_DAE_H_
#define MAKE_PDDL_DAE_H_

#include <utils/eoParser.h>
#include <utils/eoState.h>
#include <daex.h>

daex::pddlLoad & do_make_pddl (eoParser& _parser, eoState& _state /* FIXME unsued bariable _state */)
{

    std::string domain = _parser.createParam(
            (std::string)"./benchmarks/zenoMiniMulti/domain-zeno-time-miniRisk.pddl",
            "domain", "PDDL domain file", 'D', "Problem", true ).value();

    std::string instance = _parser.createParam(
            (std::string)"./benchmarks/zenoMiniMulti/zenoMiniMulti.pddl",
            "instance", "PDDL instance file", 'I', "Problem", true ).value();

    bool is_sequential = _parser.createParam(
            (bool)false,
            "sequential", "Is the problem a sequential one?", 'q', "Problem", true ).value();

    daex::pddlLoad * pddl = new daex::pddlLoad ( domain, instance, SOLVER_YAHSP, HEURISTIC_H1, is_sequential );

    return *pddl;
}

#endif // MAKE_PDDL_DAE_H_

