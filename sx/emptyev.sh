#!/bin/bash

YEAR=$1
N=$2

for ((i=0; i<${N}; i++))
do
  cat data/l30 >> db/events-${YEAR}.db
done
