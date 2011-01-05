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

# IPC6
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/tempo-sat/" $runs "tempo-sat"
sge_submit_array_ipc "/tools/pddl/ipc/ipc2008/seq-sat/" $runs "seq-sat"

# IPC3 TEMPORAL

# domain=/tools/pddl/ipc/IPC3/Tests2/Rovers/SimpleTime/STRover.pddl
# instance= /tools/pddl/ipc/IPC3/Tests2/Rovers/SimpleTime/pfile1 to pfile20
#sge_submit_array_ipc "/tools/pddl/ipc/IPC3/Tests2/Rovers/SimpleTime" $runs "STRIPS"

# domain=/tools/pddl/ipc/IPC3/Tests1/ZenoTravel/SimpleTime/zenoSimpleTime.pddl
# instance= /tools/pddl/ipc/IPC3/Tests1/ZenoTravel/SimpleTime/pfile1 to pfile20
#sge_submit_array_ipc "/tools/pddl/ipc/IPC3/Tests1/ZenoTravel/SimpleTime" $runs "TEMPORAL"

# domain=/tools/pddl/ipc/IPC3/Tests2/Satellite/SimpleTime/simpleTimeSat.pddl
# instance=/tools/pddl/ipc/IPC3/Tests2/Satellite/SimpleTime/pfile1 to pfile20
#sge_submit_array_ipc "/tools/pddl/ipc/IPC3/Tests2/Satellite/SimpleTime" $runs "STRIPS"


# IPC4 STRIPS

# domain=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/NONTEMPORAL/STRIPS/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/NONTEMPORAL/STRIPS/P01_AIRPORT1_P1.PDDL to P50_AIRPORT5MUC_P15.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/NONTEMPORAL/STRIPS" $runs "STRIPS"

# domain=/tools/pddl/ipc/IPC4/DOMAINS/PSR/SMALL/STRIPS/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PSR/SMALL/STRIPS/P01_S2_N1_L2_F50.PDDL to P50_S107_N6_L2_F70.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PSR/SMALL/STRIPS" $runs "STRIPS"

# domain=/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/STRIPS/STRIPS/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/STRIPS/STRIPS/P01_PFILE1.PDDL to P36_HC_PFILE16.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/STRIPS/STRIPS" $runs "STRIPS"

# IPC5 STRIPS
# PREMISSION DENIED !!!! sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/OPENSTACKS/STRIP" "--sequential=1" $runs "STRIPS"
#sge_submit_array_ipc "/tools/pddl/ipc/IPC5/DOMAINS/PATHWAYS/STRIP" $runs "STRIPS"

# IPC3 STRIPS

# domain=/tools/pddl/ipc/IPC3/Tests2/Rovers/Strips/StripsRover.pddl
# instance= /tools/pddl/ipc/IPC3/Tests2/Rovers/Strips/pfile1 to pfile20
#sge_submit_array_ipc "/tools/pddl/ipc/IPC3/Tests2/Rovers/Strips" $runs "STRIPS"

# domain=/tools/pddl/ipc/IPC3/Tests1/ZenoTravel/Strips/zenotravelStrips.pddl
# instance=/tools/pddl/ipc/IPC3/Tests1/ZenoTravel/Strips/pfile1 to pfile20
#sge_submit_array_ipc "/tools/pddl/ipc/IPC3/Tests1/ZenoTravel/Strips" $runs "STRIPS"


# domain=/tools/pddl/ipc/IPC3/Tests2/Satellite/Strips/stripsSat.pddl
# instance=/tools/pddl/ipc/IPC3/Tests2/Satellite/Strips/pfile1 to pfile20
#sge_submit_array_ipc "/tools/pddl/ipc/IPC3/Tests2/Satellite/Strips" $runs "STRIPS"

# domain=/tools/pddl/ipc/IPC3/Tests3/FreeCell/Strips/free.pddl
# instance=/tools/pddl/ipc/IPC3/Tests3/FreeCell/Strips/pfile1 to pfile20
#sge_submit_array_ipc "/tools/pddl/ipc/IPC3/Tests3/FreeCell/Strips" $runs "STRIPS"

