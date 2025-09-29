#!/bin/bash
clear
cd /storage-home/s/sb121/comp530/A2/Build
rm core*
echo 4 | ~/scons/bin/scons-3.1.2
bin/recordUnitTest
gdb bin/recordUnitTest core*