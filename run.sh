#!/bin/bash
#B136013
#define variables
DATA_FOLDER=./data
GRAPHS_FOLDER=./graphs
# RESERVATION="-q R381703"

#make required folders
mkdir -p $DATA_FOLDER
mkdir -p $GRAPHS_FOLDER

#keep data from previous project
export GLOBIGNORE=$DATA_FOLDER/jmax.tsv:$DATA_FOLDER/results_coursework1.txt:$DATA_FOLDER/data_coursework1.tsv
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
qsub $RESERVATION loops2.pbs
