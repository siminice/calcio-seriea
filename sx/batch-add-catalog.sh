#!/bin/bash

YEAR=$1
LETTER=$2

for LETTER in A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
do
  bin/add-catalog catalogs/players.dat tmp/${YEAR}/${LETTER}.in
done
