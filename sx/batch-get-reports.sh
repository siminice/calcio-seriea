#!/bin/bash

scripts/get-reports.sh 1941 27
scripts/get-reports.sh 1942 29
scripts/get-reports.sh 1946 31
scripts/get-reports.sh 1947 35

for ((y=1948; y<2015; y++))
do
  i=$((2*($y-1929)+1))
  echo "$y -> $i"
  scripts/get-reports.sh $y $i
done

scripts/get-reports.sh 2015 174
scripts/get-reports.sh 2016 176

#29 ->   1
#39 ->  21
#49 ->  41
#59 ->  61
#69 ->  81
#79 -> 101
#89 -> 121
#99 -> 141
#09 -> 161
#14 -> 171
