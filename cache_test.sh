#!/bin/bash

FILES="traces-short/tr1"
CONFIG="config-files/*"
RESULTS=results
mkdir "results";

for f in $FILES
do
    for c in $CONFIG
    do
      config="$(basename ${c%.*})"
      trace=$(basename $f)
      echo "-----------------------------------------------------------------" >> "results/$trace.$config"
      echo "       $trace.$config     Simulation Results" >> "results/$trace.$config"  
      echo "-----------------------------------------------------------------" >> "results/$trace.$config"
          cat $f | ./cache_sim  $c >> "results/$trace.$config"
    done
done
