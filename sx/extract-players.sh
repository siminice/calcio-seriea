#!/bin/bash

YEAR=$1
LETTER=$2
PAGES=$3
scripts/get-catalog.sh ${YEAR} ${LETTER} ${PAGES}

while read link
do
  ./scripts/get-player.sh ${link}
done < tmp/${YEAR}/${LETTER}.links | tee tmp/${YEAR}/${LETTER}.in

