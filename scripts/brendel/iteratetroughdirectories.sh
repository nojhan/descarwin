for d in `ls ./*/ -d`
    do

    echo $d

    cd $d
    RunAllExperiments_local.sh  p p 1 30 11 1800
    cd ..



    done


