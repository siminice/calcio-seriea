#!/bin/bash

declare -A months
months=( \
["-"]="00" \
["gennaio"]="01" \
["febbraio"]="02" \
["marzo"]="03" \
["aprile"]="04" \
["maggio"]="05" \
["giugno"]="06" \
["luglio"]="07" \
["agosto"]="08" \
["settembre"]="09" \
["ottobre"]="10" \
["novembre"]="11" \
["dicembre"]="12" \
)

declare -A ctty
ctty=( \
["Argentina"]="ARG" \
["Austria"]="AUT" \
["Belgio"]="BEL" \
["Brasile"]="BRA" \
["Cecoslovacchia"]="CZE" \
["Danimarca"]="DEN" \
["Francia"]="FRA" \
["Germania"]="GER" \
["Inghilterra"]="ENG" \
["Italia"]="ITA" \
["Jugoslavia"]="YUG" \
["Olanda"]="NED" \
["Paraguay"]="PAR" \
["Romania"]="ROM" \
["Scozia"]="SCO" \
["Svezia"]="SWE" \
["Svizzera"]="SUI" \
["Ungheria"]="HUN" \
["Uruguay"]="URU" \
)

URL="http://calcio-seriea.net"
INDEX=$(echo $1 | cut -d '/' -f 4)

curl -s "${URL}/$1" > tmp/p/player-${INDEX}.html

FIRSTNAME1=$(./scripts/get-tag.sh ${INDEX} NOME)
FIRSTNAME2=$(./scripts/get-tag2.sh ${INDEX} NOME)
LASTNAME1=$(./scripts/get-tag.sh ${INDEX} COGNOME)
LASTNAME2=$(./scripts/get-tag2.sh ${INDEX} COGNOME)
NAT=$(./scripts/get-tag2.sh ${INDEX} NAZIONALIT)
DOB=$(./scripts/get-tag.sh ${INDEX} DATA)
POB=$(./scripts/get-tag.sh ${INDEX} LUOGO)
XOB=$(echo ${DOB} | cut -d ' ' -f 1)
printf -v ZOB '%02d' "${XOB}"
MOB=$(echo ${DOB} | cut -d ' ' -f 2)
YOB=$(echo ${DOB} | cut -d ' ' -f 3)
KOB=$(echo ${POB} | cut -d '(' -f 1 | sed -e 's/[ \t]*$//')
JOB=$(echo ${POB} | sed -e 's/.*(\(.*\))/\1/')
COB="ITA"

DOB0="0000-00-00"

if [ -z "${DOB}" ]; then
  DOB=${DOB0}
fi
if [ -z "${YOB}" ]; then
  YOB="0000"
fi
if [ -z "${MOB}" ]; then
  MOB="-"
fi
if [ -z "${ZOB}" ]; then
  ZOB="00"
fi
if [ -z "${POB}" ]; then
  POB=" "
fi
echo ${POB} > pob
if grep -q "(" pob; then
if [ ! -z "${JOB}" ]; then
  POB=${KOB}
  COB=${JOB}
fi
fi

FIRSTNAME=${FIRSTNAME1}
if [ -z "${FIRSTNAME1}" ]; then
  FIRSTNAME=${FIRSTNAME2}
fi

LASTNAME=${LASTNAME1}
if [ -z "${LASTNAME1}" ]; then
  LASTNAME=${LASTNAME2}
fi

CTY=${ctty[${NAT}]}
if [ -z "${CTY}" ]; then
  CTY=" "
fi

echo "${FIRSTNAME} ${LASTNAME},${CTY},${YOB}-${months[${MOB}]}-${ZOB},${POB},${COB}"
