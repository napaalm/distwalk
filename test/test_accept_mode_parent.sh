#!/bin/bash

# ASSUMPTION: round-robin load balacing

. common.sh

tmp=$(mktemp /tmp/test_accept_mode_parent-XXX.dat)

node_bg -a parent --nt 3 &> $tmp

client -C 1000 -n 3 --ns 3

cat $tmp | grep assigned | head -1 | grep -q "assigned to connw-0"
cat $tmp | grep assigned | head -2 | tail -1 | grep -q "assigned to connw-1"
cat $tmp | grep assigned | head -3 | tail -1 | grep -q "assigned to connw-2"

kill_all SIGINT
rm $tmp


node_bg -a parent --nt 2

client -C 1000000 -p 1000000 --nt 4 &> $tmp

elapsed=($(cat $tmp | grep 'elapsed:' | sed -e 's/.*elapsed: //; s/ us.*//'))

[ ${elapsed[0]} -gt 999000 ] && [ $elapsed -lt 1001000 ]
[ ${elapsed[1]} -gt 999000 ] && [ $elapsed -lt 1001000 ]
[ ${elapsed[2]} -gt 1999000 ] && [ $elapsed -lt 2001000 ]
[ ${elapsed[3]} -gt 1999000 ] && [ $elapsed -lt 2001000 ]

kill_all SIGINT
rm $tmp
