#!/bin/sh

OK="\\033[1;32m"
NOK="\\033[1;31m"
RAZ="\\033[0;39m"

build_scripts=$(ls -1 --color=none build_*)

max=0
for b in $build_scripts; do
    if [[ ${#b} > $max ]]; then
        max=${#b}
    fi
done

for b in $build_scripts; do
    printf "%${max}s ... " "$b"
    ./$b 1>/dev/null 2>/dev/null
    if [[ $? == 0 ]]; then
        printf "${OK} OK${RAZ}\n"
    else
        printf "${NOK}NOK${RAZ}\n"
    fi
done

