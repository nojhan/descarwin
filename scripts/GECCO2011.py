#!/usr/bin/env python

import optparse, logging, sys, os
from datetime import datetime
import common

logger = logging.getLogger("GECCO2011")

### MAIN SETTINGS ###

# name - domain - instance
SAMPLES = [("IPC6_SEQ_ELEVATORS_12",
            "/tools/pddl/ipc/ipc2008/seq-sat/elevators-strips/p12-domain.pddl",
            "/tools/pddl/ipc/ipc2008/seq-sat/elevators-strips/p12.pddl"),
           ("IPC6_TEMPO_OPENSTACKS_17",
            "/tools/pddl/ipc/ipc2008/tempo-sat/openstacks-strips/p17-domain.pddl",
            "/tools/pddl/ipc/ipc2008/tempo-sat/openstacks-strips/p17.pddl")]

### MAIN SETTINGS ENDS ###

PATTERN_TIME_FILENAME = "%(TIMEDIR)s/%(NAME)s_%(FIELD)s.time.%(NUM)s"
PATTERN_RES_FILENAME = "%(RESDIR)s/%(NAME)s_%(FIELD)s.out.%(NUM)s"
PATTERN_PLAN_FILENAME = "%(RESDIR)s/%(NAME)s_%(FIELD)s.soln.%(NUM)s"

PATTERN_CMD = \
    "/usr/bin/time -v -o %(TIME_FILENAME)s "\
    "./dae "\
    "--domain=%(DOMAIN)s "\
    "--instance=%(INSTANCE)s "\
    "--plan-file= %(PLAN_FILENAME)s "\
    "--runs-max=%(RUNMAX)s "\
    "--popSize=%(POPSIZE)s "\
    "--offsprings=%(OFFSPRINGS)s "\
    "--max-seconds=%(MAXSECONDS)s "\
    "--gen-steady=%(GENSTEADY)s "\
    "--parallelize-loop=%(LOOP)s "\
    "--parallelize-nthreads=%(THREADS)s "\
    "--parallelize-dynamic=%(DYNAMIC)s "\
    "> %(RES_FILENAME)s"

parser = optparse.OptionParser()
parser.add_option('-e', '--execute', action='store_true', default=False, help='execute experiences')
parser.add_option('-D', '--dynamic', action='store_true', default=False, help='use the dynamic scheduler in openmp')
parser.add_option('-p', '--plot', action='store_true', default=False, help='plot data')
parser.add_option('-N', '--nruns', type='int', default=11, help='give here a number of runs')
topic = str(datetime.today())
for char in [' ', ':', '-', '.']: topic = topic.replace(char, '_')
parser.add_option('-t', '--topic', default=topic, help='give here a topic name used to create the folder')
options = common.parser(parser)

def main():

    resdir = "%s/Res" % options.topic
    timedir = "%s/Time" % options.topic

    # create needed directories
    if options.execute:
        for d in [resdir, timedir]:
            try:
                os.makedirs(d)
            except OSError:
                pass


    # EXPS on IPC6_SEQ_ELEVATORS_12 & IPC6_TEMPO_OPENSTACKS_17
    # steadyState=50
    # 1) popsize=48 & runmax=1 & maxseconds=0
    # 2) RESTART case: popsize=96 & runmax=0 & maxseconds=1799
    # foreach nthreads: 1, 24, 48
    # repeat 11 times

    for field, popsize, runmax, maxseconds in [("PROC", 48, 1, 0),
                                               ("RESTART_PROC", 96, 0, 1799)]:
        for name, domain, instance in SAMPLES:
            plotdata = []
            for num in range(1, options.nruns+1):
                subdata = []
                for nthreads in [1, 24, 48]:
                    field_name = "%s_%s_%d" % (field, "DYNAMIC" if options.dynamic else "STATIC", popsize)
                    time_filename = PATTERN_TIME_FILENAME % {"TIMEDIR": timedir, "NAME": name, "FIELD": field_name, "NUM": num}
                    res_filename = PATTERN_RES_FILENAME % {"RESDIR": resdir, "NAME": name, "FIELD": field_name, "NUM": num}
                    plan_filename = PATTERN_PLAN_FILENAME % {"RESDIR": resdir, "NAME": name, "FIELD": field_name, "NUM": num}
                    cmd = PATTERN_CMD % {"DOMAIN": domain,
                                         "INSTANCE": instance,
                                         "LOOP": 1,
                                         "DYNAMIC": 1 if options.dynamic else 0,
                                         "THREADS": nthreads,
                                         "RUNMAX": runmax,
                                         "POPSIZE": popsize,
                                         "OFFSPRINGS": popsize*7,
                                         "MAXSECONDS": maxseconds,
                                         "GENSTEADY": 50,
                                         "TIME_FILENAME": time_filename,
                                         "RES_FILENAME": res_filename,
                                         "PLAN_FILENAME": plan_filename,
                                         }
                    logger.debug(cmd)
                    if options.execute:
                        os.system( cmd )
                    if options.plot:
                        try:
                            subdata.append(open(time_filename).readlines()[4].split()[-1])
                        except IOError:
                            pass

                if options.plot:
                    #logger.info(subdata)
                    if len(subdata):
                        plotdata.append(subdata)

            logger.info(plotdata)


    # EXPS on IPC6_SEQ_ELEVATORS_12 & IPC6_TEMPO_OPENSTACKS_17
    # nthreads=48 & steadyState=50
    # 1) runmax=1 & maxseconds=0
    # 2) RESTART case: runmax=0 & maxseconds=1799
    # foreach popSize: 1, 24, 48, 72, 96
    # repeat 11 times

    for field, runmax, maxseconds in [("POP", 1, 0),
                                      ("RESTART_POP", 0, 1799)]:
        for name, domain, instance in SAMPLES:
            plotdata = []
            for num in range(1, options.nruns+1):
                subdata = []
                for popsize in [1, 24, 48, 72, 96]:
                    field_name = "%s_%s_%d" % (field, "DYNAMIC" if options.dynamic else "STATIC", popsize)
                    time_filename = PATTERN_TIME_FILENAME % {"TIMEDIR": timedir, "NAME": name, "FIELD": field_name, "NUM": num}
                    res_filename = PATTERN_RES_FILENAME % {"RESDIR": resdir, "NAME": name, "FIELD": field_name, "NUM": num}
                    plan_filename = PATTERN_PLAN_FILENAME % {"RESDIR": resdir, "NAME": name, "FIELD": field_name, "NUM": num}
                    cmd = PATTERN_CMD % {"DOMAIN": domain,
                                         "INSTANCE": instance,
                                         "LOOP": 1,
                                         "DYNAMIC": 1 if options.dynamic else 0,
                                         "THREADS": 48,
                                         "RUNMAX": runmax,
                                         "POPSIZE": popsize,
                                         "OFFSPRINGS": popsize*7,
                                         "MAXSECONDS": maxseconds,
                                         "GENSTEADY": 50,
                                         "TIME_FILENAME": time_filename,
                                         "RES_FILENAME": res_filename,
                                         "PLAN_FILENAME": plan_filename,
                                         }
                    logger.debug(cmd)
                    if options.execute:
                        os.system( cmd )
                    if options.plot:
                        try:
                            subdata.append(open(time_filename).readlines()[4].split()[-1])
                        except IOError:
                            pass

                if options.plot:
                    #logger.info(subdata)
                    if len(subdata):
                        plotdata.append(subdata)

            logger.info(plotdata)

# when executed, just run main():
if __name__ == '__main__':
    logger.debug('### script started ###')
    main()
    logger.debug('### script ended ###')
