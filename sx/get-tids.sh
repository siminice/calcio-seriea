#!/bin/bash

YEAR=$1
ID=$2

curl "http://calcio-seriea.net/info/${YEAR}/" > tmp/${YEAR}/tids.html

cat tmp/${YEAR}/tids.html | grep "/info/${YEAR}/" -A 4
