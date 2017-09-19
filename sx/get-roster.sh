#!/bin/bash

YEAR=${1:-1929}
TEAM=${2:-1}

curl -s "http://calcio-seriea.net/rose/${YEAR}/${TEAM}/" > "tmp/${YEAR}/t${TEAM}.html"

cat "tmp/${YEAR}/t${TEAM}.html" | grep "scheda_gioca" -A 1 | grep "</a>" | cut -d '<' -f 1 | sed -e 's/^[ \t]*//' | tee "tmp/${YEAR}/t${TEAM}.in"
