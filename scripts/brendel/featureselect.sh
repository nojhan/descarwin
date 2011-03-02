
	for (( j = 1 ; j<= 30 ;j++))
		do
	if (( $j < 10 )); then
	    featureselector p0$j-domain.pddl p0$j.pddl
	else
            featureselector p$j-domain.pddl  p$j.pddl
        fi
                      
	done

