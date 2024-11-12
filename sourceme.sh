#!/bin/bash

if [[ "$#" -eq 0 ]]; then
    alias a='bash sourceme.sh run'
    alias b='bash sourceme.sh build'
    alias a
    alias b
else
    if [[ "$1" == "build" ]]; then
        gcc -ggdb -Wall -Wextra -O3 DataTable.c main.c -o app
    fi

    if [[ "$1" == "run" ]]; then
        shift # remove 'run' form arg list before passing them all to program
        ./app "$@"
    fi
fi
