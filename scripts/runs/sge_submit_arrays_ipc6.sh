#!/bin/bash
#####################
# GENERIC FUNCTIONS #
#####################

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
    domains=`find $base_rep -type f -name *-domain.pddl | sed 's#^.#/#' | sort  | grep -v adl | grep -v numeric | grep -v cybersec`
    #domains=`find $base_rep -type f -name *-domain.pddl | sed 's#^.#/#' | sort  | grep -v adl | grep -v numeric | grep -v cybersec | grep elevators-strips | grep p0`
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

            # submit an array of *runs jobs
            # additional arguments are in $options
            cmd1="qsub -l h_rt=00:30:00 -q all.q@@ls -b y -N dae_${d}_${i}      -cwd -o $res/data/ -e $res/logs/ -t 1-$runs -S /bin/bash ./run_wrapper.sh $domain $instance $options $res"
            cmd2="qsub -l h_rt=00:30:00 -q all.q@@ls -b y -N jack_${d}_${i} -cwd -o $res/data/ -e $res/logs/ -t 1-$runs -S /bin/bash ./run_wrapper_jack.sh $domain $instance $options $res"
            
            echo $cmd1
            echo $cmd2
            #./run_wrapper.sh $domain $instance $options
            #./run_wrapper_jack.sh $domain $instance $options
            $cmd1
            $cmd2

            cptr=$((cptr+1))
        else
            echo "ERROR, instance or domain files cannot be open: $instance"
        fi
    done # domain

    echo "$((cptr*runs)) runs submitted"
}


#######################
# CURRENT EXPERIMENTS #
#######################

# nb of differents runs for each instance
runs=11

# IPC6
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/tempo-sat/" 0 $runs "tempo-sat"
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/seq-sat/" 1 $runs "seq-sat"

