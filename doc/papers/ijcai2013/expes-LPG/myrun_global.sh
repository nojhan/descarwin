#!/bin/bash

### PARAMETRES A MODIFIER A LA MAIN

# nom du fichier domaine
dom=domain-zeno-time-miniRisk-LPG.pddl

# nom de l'instance version LPG (sans la parenthèse finale)
prob=zeno6eMiniMulti-LPG.pddl

# nom du répertoire où les données seront sauvées
dir=zeno6

#durée d'un run en secondes : 5 minutes pour zeno6
runtime=30000

# temps limite en secondes pour un run de LPG -> peut-être 100 pour zeno6 ??
lim=120

# nombre total de runs
nbruns=100

######################################



tmp=tmp.pddl

function mkruns()
{
    gcc bin/timer.c -o bin/timer
    for ((run=1; run<=nbruns; run++)); do
	echo RUN $run  ---   $(date) >> all.log
	loop=1
	sdir=$dir/run$(printf "%02d" $run)
	mkdir -p $sdir

# on ajoute le numéro du run, qui servira pour la graine aléatoire	
	bin/timer $1 myrun_lpg.sh $lim $dom $prob $sdir $run 2>/dev/null >/dev/null
	
    done
}

function extract()
{
    cp $prob $tmp-mksp
    echo "(:metric minimize (total-time)))" >> $tmp-mksp
    cp $prob $tmp-cost
    echo "(:metric minimize (total-cost)))" >> $tmp-cost
    
    
    for ((run=1; run<=nbruns; run++)); do
	r=$(printf "%02d" $run)
	for f in $dir/run$r/*; do
	    time=$(echo $f | cut -d"+" -f 2 | cut -d "+" -f 1)
	    mksp=$(bin/validate -t 0.0001 $dom $tmp-mksp $f|awk '/^Final/{print $3}')
	    cost=$(bin/validate -t 0.0001 $dom $tmp-cost $f|awk '/^Final/{print $3}')
	    echo $time ${mksp%.*} $cost
	done > $dir/run$r.tab
    done

    rm $tmp-*
}

/bin/rm all.log
touch all.log
mkruns $runtime
extract $1
