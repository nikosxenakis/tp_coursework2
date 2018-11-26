#!/usr/bin/env python2
# check_correctness.py

import os


def get_next_string(src, contains):
    if src.startswith(contains):
        return src[src.index(contains) + len(contains):len(src)-1]
    return None


def check_correctness(data_file):

    loop1_sum = 'Loop 1 check: Sum of a is '
    loop2_sum = 'Loop 2 check: Sum of c is '
    loop1_res = float(-343021.474766)
    loop2_res = float(-25242644.603147)

    for line in data_file:
        res = get_next_string(line, loop1_sum)
        if res is not None:
            if loop1_res != float(res):
                print "Error the results are wrong in loop 1"
                exit()
        res = get_next_string(line, loop2_sum)
        if res is not None:
            if loop2_res != float(res):
                print "Error the results are wrong in loop 2"
                exit()        


#check correctness in ./data/data_coursework2.tsv 
path = './data/results_coursework2.txt'
f = open(path, 'r')
check_correctness(f)
print "The results are correct"
f.close()
