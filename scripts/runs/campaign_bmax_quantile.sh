#!/bin/bash
#####################
# GENERIC FUNCTIONS #
#####################

# TODO add ipc3

# submit arrays of jobs with SGE, on each instance available in a given directory
function sge_submit_array_ipc()
{

    # directory where to search PDDL domains and instance files 
    base_rep=$1

    # additional options to give to the solver (e.g. --sequential)
    options=$2

    # nb of differents runs for each instance
    runs=$3

    # directory where to put everything
    res=$4

    # create directories if they do not exists
    mkdir -p $res
    mkdir -p $res/data
    mkdir -p $res/logs
    mkdir -p $res/status
    mkdir -p $res/plans
    mkdir -p $res/gens

    # list the available domain files in the sub-tree of this directory
    # default: do not consider ADL, NUMERIC and CYBERSEC
    domains=`find $base_rep -type f -name *-domain.pddl | sed 's#^.#/#' | sort  | grep -v adl | grep -v numeric | grep -v cybersec|grep -v p0`
    # + only problem within [01-09], add: | grep p0`

    for domain in $domains; do

        cptr=0
      
        # in IPC6 (2008) instances files have the same names than the domains
        instance=`echo $domain | sed s/-domain//g`

        if test -r $instance && test -r $domain; then

            # parent directory
            d=`echo $domain | cut -d '/' -f 7`

            # filename without the extension
            i=`basename $instance .pddl`

            # b_max quantiles
            for q in `seq 1 1 9`; do
                # submit an array of *runs jobs
                # additional arguments are in $options
                dshort=`echo $d | head -c 4`
                cmd="qsub -l h_rt=00:30:00 -q all.q@@ls -b y -N ${dshort}_${q}_${i} -cwd -o $res/data/ -e $res/logs/ -t 1-$runs -S /bin/bash ./run_bmax_quantile.sh $domain $instance $options $res $q"
                
                echo $cmd
                #./run_bmax_quantile.sh $domain $instance $options $res $q
                $cmd

                cptr=$((cptr+1))

            done
        else
            echo "ERROR, instance or domain files cannot be open: $instance"
        fi
    done # domain

    echo "$((cptr*runs*9)) runs submitted"
}


#######################
# CURRENT EXPERIMENTS #
#######################

# nb of differents runs for each instance
runs=11

# IPC6
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/tempo-sat/elevators-strips" 0 $runs "tempo-sat/elevators-strips"
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/tempo-sat/pegsol-strips" 0 $runs "tempo-sat/pegsol-strips"
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/tempo-sat/sokoban-strips" 0 $runs "tempo-sat/sokoban-strips"

sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/seq-sat/elevators-strips" 1 $runs "seq-sat/elevators-strips"
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/seq-sat/pegsol-strips" 1 $runs "seq-sat/pegsol-strips"
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/seq-sat/sokoban-strips" 1 $runs "seq-sat/sokoban-strips"
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/seq-sat/openstacks-strips" 1 $runs "seq-sat/openstacks-strips"
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/seq-sat/scanalyzer-strips" 1 $runs "seq-sat/scanalyzer-strips"

