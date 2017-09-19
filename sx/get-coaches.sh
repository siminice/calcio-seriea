#!/bin/bash

YEAR=${1:-1929}

curl -s "http://calcio-seriea.net/allenatori/${YEAR}/1/" > "tmp/${YEAR}/coaches.html"

cat "tmp/${YEAR}/coaches.html" | grep "scheda_" | cut -d '"' -f 2 > "tmp/${YEAR}/coaches.links"
