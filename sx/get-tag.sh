#!/bin/bash

INDEX=$1
TAG=$2

cat tmp/p/player-${INDEX}.html | grep ${TAG} -A 2 | tail -n 1 | cut -d '<' -f 1 | sed -e 's/^[ \t]*//'
