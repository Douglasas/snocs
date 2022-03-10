#!/bin/bash

BASEDIR=$(pwd)
MY_WORKSPACE=$BASEDIR/my_workspace
MY_PLUGINS=$BASEDIR/plugins

rm -rf results
mkdir results

make -C PG_AI clean
make -C PG_AI

cd Simulator
./SNoCS 10 $MY_WORKSPACE $MY_PLUGINS
cd ..

mv $MY_WORKSPACE/*_out results/

rm $MY_WORKSPACE/stopsim.out
