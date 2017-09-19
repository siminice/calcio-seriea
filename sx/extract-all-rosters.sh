#!/bin/bash

YEAR=${1}
INDEX=${2}

curl -s http://calcio-seriea.net/info/${YEAR}/${INDEX} | grep 
