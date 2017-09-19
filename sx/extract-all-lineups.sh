#!/bin/bash

YEAR=$1

for LETTER in A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
do
   scripts/extract-lineups.sh ${YEAR} ${LETTER}
done
