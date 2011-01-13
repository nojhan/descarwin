domains=`ls -1|awk -F _p[0-9] '{print $1}'|uniq `

for d in $domains ; do
    for i in {0..2}{0..9} ; do 
        if [ $i == 00 ] ; then
            continue
        fi
        for r in {1..11} ; do 
             plan=`ls -1 -v ${d}_p${i}_r${r}.* |tail -n 1 2>/dev/null`
             if [ $plan != "" ] ; then
                to=`echo ${plan}|cut -d '.' -f 1`.soln
                echo $to
                cp $plan $to
             fi
         done
     done
 done

