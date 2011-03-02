for d in `ls ./mytrain/ -d`
    do
    echo $d
	for (( j = 1 ; j<= 300 ;j++))
		do
		waituntilfreecore.sh 17 1 featureselector
		if (( $j < 10 )); then
		nohup featureselector ${d}p0$j-domain.pddl ${d}p0$j.pddl | grep features > ${d}features_$j.txt&
		wc ${d}p0$j-domain.pddl | tr -d '\012'  > ${d}wcfeatures_$j.txt
		wc ${d}p0$j.pddl | tr -d '\012'  >> ${d}wcfeatures_$j.txt
		grep durative   ${d}p0$j-domain.pddl | wc | awk '{print " " $1}' | tr -d '\012' >>${d}wcfeatures_$j.txt
		echo -n " $j" >> ${d}wcfeatures_$j.txt
		else
        	nohup featureselector ${d}p$j-domain.pddl  ${d}p$j.pddl | grep features > ${d}features_$j.txt&
        	wc ${d}p$j-domain.pddl | tr -d '\012'  > ${d}wcfeatures_$j.txt 
        	wc ${d}p$j.pddl | tr -d '\012'  >> ${d}wcfeatures_$j.txt 
        	grep durative   ${d}p$j-domain.pddl | wc | awk '{print " " $1}' | tr -d '\012' >>${d}wcfeatures_$j.txt
        	echo -n " $j">> ${d}wcfeatures_$j.txt
        	
    		fi
    		done

    done


