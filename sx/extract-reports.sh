#!/bin/bash

YEAR=$1
SEASON=$(($1+1))
LOAD=${2:-no}

while read link
do
  echo ${link}
  r=$(echo ${link} | cut -d '/' -f 4)
  if [ "${LOAD}" = "load" ]; then
    curl -s http://calcio-seriea.net${link} > tmp/${YEAR}/rep-$r.html
  fi
  bin/add-lineups ${YEAR} ${r} > log-$r
done < tmp/${YEAR}/reports.in
