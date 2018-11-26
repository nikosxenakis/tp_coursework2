#!/bin/bash
#
#define variables
DATA_FOLDER=./data
GRAPHS_FOLDER=./graphs

#make required folders
mkdir -p $DATA_FOLDER
mkdir -p $GRAPHS_FOLDER

#keep data from previous project
export GLOBIGNORE=$DATA_FOLDER/jmax.tsv:$DATA_FOLDER/results_coursework1.txt:$DATA_FOLDER/data_coursework2.tsv
#clean graphs
rm -rf $GRAPHS_FOLDER/*
#clean data
rm -rf $DATA_FOLDER/*
export GLOBIGNORE=
#clean objective files and target file
make clean

#make loops2
make loops2

#submit job
qsub loops2.pbs
