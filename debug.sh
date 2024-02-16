#!/bin/bash

if [[ $# -eq 1 ]]
then
    ./sdriver.pl -t "$1" -s ./shell
else
    echo "Usage: ./debug.sh tests/test[your_test].txt"
fi