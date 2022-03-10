#!/bin/bash

BASEDIR=$(pwd)
MY_WORKSPACE=$BASEDIR/my_workspace
MY_PLUGINS=$BASEDIR/plugins

rm -rf results
mkdir results

for i in {0..2}
do

  python3 generate_constant.py

  make -C PG_AI clean
  make -C PG_AI


  cd Simulator
  ./SNoCS 10 $MY_WORKSPACE $MY_PLUGINS
  cd ..

  mkdir results/$i

  mv $MY_WORKSPACE/*out results/$i/
  mv $MY_WORKSPACE/Results/ results/$i/Results/

done
