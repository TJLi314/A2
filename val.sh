#!/bin/bash
clear
cd /storage-home/s/sb121/comp530/A2/Build
echo 4 | ~/scons/bin/scons-3.1.2
if [ -n "$1" ]; then
    valgrind --leak-check=yes --show-leak-kinds=all --track-origins=yes --log-file="valgrind-out.txt" bin/recordUnitTest > "../$1"
else
    valgrind --leak-check=yes --show-leak-kinds=all --track-origins=yes --log-file="valgrind-out.txt" bin/recordUnitTest
fi