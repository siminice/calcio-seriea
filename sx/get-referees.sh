#!/bin/bash

YEAR=${1:-1929}

curl -s "http://calcio-seriea.net/arbitri/${YEAR}/1/" > "tmp/${YEAR}/refs.html"

cat "tmp/${YEAR}/refs.html" | grep "scheda_" | cut -d '"' -f 2 > "tmp/${YEAR}/refs.links"
