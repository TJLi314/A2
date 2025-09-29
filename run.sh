#!/bin/bash
clear
cd /storage-home/s/sb121/comp530/A2/Build
echo 4 | ~/scons/bin/scons-3.1.2
if [ -n "$1" ]; then
    bin/recordUnitTest > "../$1"
else
    bin/recordUnitTest
fi