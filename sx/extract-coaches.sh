#!/bin/bash

YEAR=$1

scripts/get-coaches.sh ${YEAR}

while read link
do
  ./scripts/get-player.sh ${link}
done < tmp/${YEAR}/coaches.links | tee tmp/${YEAR}/coaches.in
