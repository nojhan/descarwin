#!/usr/bin/env sh

# pop=48 run=1 steadyState=50 
# On fait varier le nbre de coeurs : 1, 24, 48
# On répète l'expérience 11 fois

NAME="IPC6_SEQ_ELEVATORS_12"
DOMAIN="/tools/pddl/ipc/ipc2008/seq-sat/elevators-strips/p12-domain.pddl"
INSTANCE="/tools/pddl/ipc/ipc2008/seq-sat/elevators-strips/p12.pddl"

OFFSPRINGS=336
POPSIZE=48
RUNMAX=1


NUM=1

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=2

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=3

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=4

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=5

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=6

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=7

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=8

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=9

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=10

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=11

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


#----------------------------------------------------------------------------

NAME="IPC6_TEMPO_OPENSTACKS_17"
DOMAIN="/tools/pddl/ipc/ipc2008/tempo-sat/openstacks-strips/p17-domain.pddl"
INSTANCE="/tools/pddl/ipc/ipc2008/tempo-sat/openstacks-strips/p17.pddl"

OFFSPRINGS=336
POPSIZE=48
RUNMAX=1


NUM=1

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=2

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=3

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=4

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=5

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=6

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=7

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=8

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=9

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=10

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=11

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


#----------------------------------------------------------------------------

# NB core=48 run=1 steadyState=50 
# On fait varier la population : 1, 24, 48, 72, 96
# On répète l'expérience 11 fois

NAME="IPC6_SEQ_ELEVATORS_12"
DOMAIN="/tools/pddl/ipc/ipc2008/seq-sat/elevators-strips/p12-domain.pddl"
INSTANCE="/tools/pddl/ipc/ipc2008/seq-sat/elevators-strips/p12.pddl"

NUM=1

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=2

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=3

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=4

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM

NUM=5

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM



NUM=6

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=7

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=8

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=9

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=10

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=11

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


#----------------------------------------------------------------------------

# NB core=48 run=1 steadyState=50 
# On fait varier la population : 1, 24, 48, 72, 96
# On répète l'expérience 11 fois

NAME="IPC6_TEMPO_OPENSTACKS_17"
DOMAIN="/tools/pddl/ipc/ipc2008/tempo-sat/openstacks-strips/p17-domain.pddl"
INSTANCE="/tools/pddl/ipc/ipc2008/tempo-sat/openstacks-strips/p17.pddl"

NUM=1

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=2

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=3

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=4

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM

NUM=5

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM



NUM=6

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=7

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=8

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=9

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=10

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


NUM=11

/usr/bin/time -v -o ${NAME}_POP_1.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=1 --offsprings=7 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_1.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_24.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=24 --offsprings=168 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_24.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_48.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=48 --offsprings=336 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_48.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_72.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=72 --offsprings=504 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_72.out.$NUM

/usr/bin/time -v -o ${NAME}_POP_96.time.$NUM ./dae --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=1 --popSize=96 --offsprings=672 --parallelize-loop=1 --parallelize-nthreads=48 > ${NAME}_POP_96.out.$NUM


#----------------------------------------------------------------------------

# pop=96 run=1 steadyState=50 mais restart infini pendant 30 mn.
# On fait varier le nbre de coeurs : 1, 24, 48
# On répète l'expérience 11 fois

NAME="IPC6_SEQ_ELEVATORS_12_RESTART"
DOMAIN="/tools/pddl/ipc/ipc2008/seq-sat/elevators-strips/p12-domain.pddl"
INSTANCE="/tools/pddl/ipc/ipc2008/seq-sat/elevators-strips/p12.pddl"

OFFSPRINGS=672
POPSIZE=96
RUNMAX=0


NUM=1

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=2

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=3

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=4

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=5

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=6

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=7

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=8

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=9

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=10

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=11

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM



# pop=96 run=1 steadyState=50 mais restart infini pendant 30 mn.
# On fait varier le nbre de coeurs : 1, 24, 48
# On répète l'expérience 11 fois


NAME="IPC6_TEMPO_OPENSTACKS_17_RESTART"
DOMAIN="/tools/pddl/ipc/ipc2008/tempo-sat/openstacks-strips/p17-domain.pddl"
INSTANCE="/tools/pddl/ipc/ipc2008/tempo-sat/openstacks-strips/p17.pddl"

OFFSPRINGS=672
POPSIZE=96
RUNMAX=0



NUM=1

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=2

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=3

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=4

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=5

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=6

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=7

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=8

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=9

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=10

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


NUM=11

THREADS=1
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=24
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM

THREADS=48
/usr/bin/time -v -o ${NAME}_PROC_${THREADS}.time.$NUM ./dae --max-seconds=1799 --domain=$DOMAIN --instance=$INSTANCE --plan-file=${NAME} --runs-max=$RUNMAX --popSize=$POPSIZE --offsprings=$OFFSPRINGS --parallelize-loop=1 --parallelize-nthreads=${THREADS} > ${NAME}_PROC_${THREADS}.out.$NUM


