#!/bin/bash

YEAR=$1

cd data
emptydb a.${YEAR}
cd ..
mv data/lineups-${YEAR}.db db/
