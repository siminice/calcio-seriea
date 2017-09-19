#!/bin/bash

YEAR=${1:-1929}
LETTER=${2:-A}
PAGES=${3:-1}
curl -s "http://calcio-seriea.net/giocatori/${YEAR}/1/${LETTER}/" > "tmp/${YEAR}/${LETTER}.html"
if [ "${PAGES}" = "2" ]; then
  curl -s "http://calcio-seriea.net/giocatori/${YEAR}/1/${LETTER}/?pagina=2" >> "tmp/${YEAR}/${LETTER}.html"
fi
cat "tmp/${YEAR}/${LETTER}.html" | grep "scheda_" | cut -d '"' -f 2 > "tmp/${YEAR}/${LETTER}.links"
