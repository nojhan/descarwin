#!/bin/sh

OK="\\033[1;32m"
NOK="\\033[1;31m"
RAZ="\\033[0;39m"

for b in $(ls -1 --color=none build_*); do
    echo -ne "$b ...\t"
    sh $b 2>/dev/null 1>/dev/null
    if [[ $? == 0 ]]; then
        printf "${OK}OK${RAZ}\n"
    else
        printf "${NOK}NOK${RAZ}\n"
    fi
done
