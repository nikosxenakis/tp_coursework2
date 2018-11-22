#!/bin/bash
#
#define variables
DATA_FOLDER=./data
GRAPHS_FOLDER=./graphs

#make required folders
mkdir -p $DATA_FOLDER
mkdir -p $GRAPHS_FOLDER

#clean data
#keep data from previous project
export GLOBIGNORE=$DATA_FOLDER/jmax.tsv:$DATA_FOLDER/results_coursework1.txt:$DATA_FOLDER/data_coursework2.tsv
rm -rf $GRAPHS_FOLDER/*
rm -rf $DATA_FOLDER/*
export GLOBIGNORE=
make clean

#make loops2
make loops2

#submit job
qsub loops2.pbs
