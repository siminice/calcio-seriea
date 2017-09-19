#!/bin/bash

YEAR=${1:-1929}
INDEX=${2:-1}

curl -s "http://calcio-seriea.net/calendario/${YEAR}/${INDEX}/1/" > "tmp/${YEAR}/cal1.html"
curl -s "http://calcio-seriea.net/calendario/${YEAR}/${INDEX}/2/" > "tmp/${YEAR}/cal2.html"

cat "tmp/${YEAR}/cal1.html" | grep "tabellini" | cut -d '"' -f 10 >  "tmp/${YEAR}/reports.in"
cat "tmp/${YEAR}/cal2.html" | grep "tabellini" | cut -d '"' -f 10 >> "tmp/${YEAR}/reports.in"

while read link
do
  N=$(echo ${link} | cut -d '/' -f 4)
  echo "${link}"
  curl -s "http://calcio-seriea.net${link}" > "tmp/${YEAR}/rep-${N}.html"
done < "tmp/${YEAR}/reports.in"
