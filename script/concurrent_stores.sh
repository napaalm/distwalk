#!/bin/bash

../src/client -s 1 -S 100000 &
../src/client -s 1 -S 100000 &
../src/client -s 1 -S 1000000 &
wait
