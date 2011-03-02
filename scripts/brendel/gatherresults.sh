for d in `ls ./*/ -d`
    do

#    echo $d

    #cd $d
    #RunAllExperiments_local.sh  p p 1 30 1 1800
    # cd ..


    for ((i=1;i<31;i++))
	do
	for ((j=1;j<12;j++))
	    do
	    f=${d}Res$i/res$j/best.xg
	    if [ -s $f ];
	    then
		echo -n $d " " $i " " $j " "
		less $f  | tail -1
	    fi
	    done
	done
    done


