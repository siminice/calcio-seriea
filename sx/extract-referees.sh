#!/bin/bash

YEAR=$1

scripts/get-referees.sh ${YEAR}

while read link
do
  ./scripts/get-player.sh ${link}
done < tmp/${YEAR}/refs.links | tee tmp/${YEAR}/refs.in

