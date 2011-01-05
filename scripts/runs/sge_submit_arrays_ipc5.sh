#!/bin/bash
#####################
# GENERIC FUNCTIONS #
#####################

# submit arrays of jobs with SGE, on each instance available in a given directory
function sge_submit_array_ipc()
{

    # directory where to search PDDL domains and instance files 
    base_rep=$1

    # nb of differents runs for each instance
    runs=$2

    # directory where to put everything
    res=$3

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
            cmd1="qsub -l h_rt=00:30:00 -q all.q@@ls -b y -N dae_${d}_${i}      -cwd -o $res/data/ -e $res/logs/ -t 1-$runs -S /bin/bash ./run_wrapper.sh $domain $instance $res"
            cmd2="qsub -l h_rt=00:30:00 -q all.q@@ls -b y -N jack_${d}_${i} -cwd -o $res/data/ -e $res/logs/ -t 1-$runs -S /bin/bash ./run_wrapper_jack.sh $domain $instance $res"
            
            echo $cmd1
            echo $cmd2
            #./run_wrapper.sh $domain $instance
            #./run_wrapper_jack.sh $domain $instance
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

# IPC5 STRIPS
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/OPENSTACKS/STRIP" $runs "STRIPS"
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/PATHWAYS/STRIP" $runs "STRIPS"

# OPENSTACKS
# domain=/tools/pddl/ipc/IPC5/DOMAINS/openstacks/Time/Strips-Time/domain_p01.pddl to domain_p20.pddl
# instance=/tools/pddl/ipc/IPC5/DOMAINS/openstacks/Time/Strips-Time/p01.pddl to p20.pddl
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/openstacks/Time/Strips-Time" $runs "tempo-sat"


# domain=/tools/pddl/ipc/IPC5/DOMAINS/openstacks/Propositional/Strips/domain_p01.pddl to domain_p30.pddl
# instance=/tools/pddl/ipc/IPC5/DOMAINS/openstacks/Propositional/Strips/p01.pddl to p30.pddl
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/openstacks/Propositional/Strips" $runs "seq-sat"

# PATHWAYS
# domain=/tools/pddl/ipc/IPC5/DOMAINS/pathways/Propositional/Strips/domain_p01.pddl to domain_p30.pddl
# instance=/tools/pddl/ipc/IPC5/DOMAINS/pathways/Propositional/Strips/p01.pddl to p30.pddl
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/pathways/Propositional/Strips" $runs "seq-sat"

# PIPESWORLD
# domain=/tools/pddl/ipc/IPC5/DOMAINS/pipesworld/Propositional/Strips/domain_p01.pddl to domain_p50.pddl
# instance=/tools/pddl/ipc/IPC5/DOMAINS/pipesworld/Propositional/Strips/p01.pddl to p50.pddl
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/pipesworld/Propositional/Strips"  $runs "seq-sat"

# ROVERS
# domain=/tools/pddl/ipc/IPC5/DOMAINS/rovers/Propositional/Strips/domain_p01.pddl to domain_p40.pddl
# instance=/tools/pddl/ipc/IPC5/DOMAINS/rovers/Propositional/Strips/p01.pddl to p40.pddl
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/rovers/Propositional/Strips" $runs "seq-sat"

# STORAGE
# domain=/tools/pddl/ipc/IPC5/DOMAINS/storage/Propositional/domain.pddl
# instance=/tools/pddl/ipc/IPC5/DOMAINS/storage/Propositional/p01.pddl to p30.pddl
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/storage/Propositional" $runs "seq-sat"

# domain=/tools/pddl/ipc/IPC5/DOMAINS/storage/Time/domain.pddl
# instance=/tools/pddl/ipc/IPC5/DOMAINS/storage/Time/p01.pddl to p30.pddl
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/storage/Time" $runs "tempo-sat"

# TPP
# domain=/tools/pddl/ipc/IPC5/DOMAINS/TPP/Propositional/domain.pddl
# instance=/tools/pddl/ipc/IPC5/DOMAINS/TPP/Propositional/p01.pddl to p30.pddl
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/TPP/Propositional" $runs "seq-sat"

# TRUCKS
# domain=/tools/pddl/ipc/IPC5/DOMAINS/trucks/Propositional/Strips/domain_p01.pddl to domain_p30.pddl
# instance=/tools/pddl/ipc/IPC5/DOMAINS/trucks/Propositional/Strips/p01.pddl to p30.pddl
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/trucks/Propositional/Strips" $runs "seq-sat"

# domain=/tools/pddl/ipc/IPC5/DOMAINS/trucks/Time/Strips-Time/domain_p01.pddl to domain_p30.pddl
# instance=/tools/pddl/ipc/IPC5/DOMAINS/trucks/Time/Strips-Time/p01.pddl to p30.pddl
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/trucks/Time/Strips-Time" $runs "tempo-sat"

