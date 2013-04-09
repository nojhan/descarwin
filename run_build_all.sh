#!/bin/sh

OK="\\033[1;32m"
NOK="\\033[1;31m"
RAZ="\\033[0;39m"

cd builds/

build_scripts=$(ls -1 --color=none build_*)

bs=()
nb=0
max=0
for b in $build_scripts; do
    nb=$((nb+1))
    bs=(${bs[@]} $b)
    if [[ ${#b} > $max ]]; then
        max=${#b}
    fi
done

items=$( for i in $(seq 0 $((nb-1)) ); do printf "${#bs[i]}_$i\n"; done | sort | cut -d "_" -f 2 )

for i in $items; do
    printf "%${max}s ... " "${bs[$i]}"
    ./${bs[$i]} 1>/dev/null 2>/dev/null
    err=$?
    if [[ $err == 0 ]]; then
        #build_dir=$(basename $(echo ${bs[i]} | sed "s,_,/,g"))
        build_dir=$(basename $(echo ${bs[i]} | sed "s,build_,,g"))
        printf "${OK} OK${RAZ} -- run_test.sh $build_dir ... "
        ../run_test.sh $build_dir 1>/dev/null 2>/dev/null
        err=$?
        if [[ $err == 0 ]]; then
            printf "${OK} OK${RAZ}\n"
        else
            printf "${NOK}NOK${RAZ} ($err)\n"
        fi
    else
        printf "${NOK}NOK${RAZ} ($err)\n"
    fi
done

