#!/usr/bin/env python2
# data_analyzer.py

import os
import matplotlib.pyplot as plt

def get_next_string(src, contains):
    if src.startswith(contains):
        return src[src.index(contains) + len(contains):len(src)-1]
    return None


def create_dots_plots(title, x_axis_title, y_axis_title, labels, x_values, y_values, alpha, legent_pos):

    fig, ax = plt.subplots()
    ax.set_xlabel(x_axis_title)
    ax.set_ylabel(y_axis_title)

    markers = ['o-', 'gs-', 'gD-']
    colors = ['#7293cb', '#84ba51', '#d35e60']

    lines = []

    i = 0
    for y_list in y_values:
        lines.append(plt.plot(x_values, y_list, markers[i], label=labels[i], linewidth=0, color=colors[i], markersize=1))
        i = i + 1

    fig.savefig('./graphs/' + str(title) + '.eps', format='eps', dpi=1000)



def create_plots(title, x_axis_title, y_axis_title, labels, x_values, y_values, alpha, legent_pos):

    fig, ax = plt.subplots()
    ax.set_xlabel(x_axis_title)
    ax.set_ylabel(y_axis_title)

    markers = ['go-', 'gs-', 'gD-']
    colors = ['#7293cb', '#84ba51', '#d35e60']

    lines = []

    i = 0
    for y_list in y_values:
        lines.append(plt.plot(x_values, y_list, markers[i], label=labels[i], linewidth=2, color=colors[i]))
        i = i + 1

    plt.legend(loc=legent_pos)

    fig.savefig('./graphs/' + str(title) + '.eps', format='eps', dpi=1000)


def median(numbers):
    numbers = sorted(numbers)
    center = len(numbers) / 2
    if len(numbers) % 2 == 0:
        return sum(numbers[center - 1:center + 1]) / 2.0
    else:
        return numbers[center]


def parse_results_coursework2(data_file):

    i = 0
    threads = None
    threads_str = 'THREADS '
    loop1_time = 'Total time for 1000 reps of loop 1 = '
    loop2_time = 'Total time for 1000 reps of loop 2 = '
    loop1 = None
    loop2 = None
    loop1_all = []
    loop2_all = []

    f= open("./data/data_coursework2.tsv","w+")

    f.write("Number of Threads\tSchedule\tLoop\tTime\n")

    for line in data_file:
        data = line.split('\t')
        res = get_next_string(line, threads_str)
        if res is not None:
            threads = res
        loop1 = get_next_string(line, loop1_time)
        if loop1 is not None:
            loop1_all.append(float(loop1))
            if len(loop1_all) == 20:
                mean = median(loop1_all)
                loop1_all = []
                f.write("%s\taffinity\t1\t%f\n" % (threads, mean))
        loop2 = get_next_string(line, loop2_time)
        if loop2 is not None:
            loop2_all.append(float(loop2))
            if len(loop2_all) == 20:
                mean = median(loop2_all)
                loop2_all = []
                f.write("%s\taffinity\t2\t%f\n" % (threads, mean))
        i = i + 1
    f.close()


def merge_coursework_results(f1, f2):

    i = 0

    f = open("./data/results.tsv","w+")

    for line in f1:
        f.write(line)

    for line in f2:
        if i > 0:
            f.write(line)
        i = i + 1

    f.close()

def analyze_jmax(data_file):
    k = 0;
    jmax = ()
    i = []

    for line in data_file:
        data = line.split('\t')
        if k > 0:
            i.append(int(data[0]))
            jmax = jmax + (int(data[1]),)
        k = k + 1

    create_dots_plots(
        "jmax",
        "Outer Loop Index (i)",
        "jmax",
        ["jmax"],
        i,
        [jmax],
        0.4,
        "upper right"
    )


def analyze_data(data_file, loop_no):

    i = 0;

    titles = ()
    schedule = ()
    guided_time = ()
    dynamic_time = ()
    affinity_time = ()

    for line in data_file:
        data = line.split('\t')
        if i == 0:
            titles = data
        else:
            if loop_no == int(data[2]):
                schedule = schedule + (str(data[1]),)
                if str(data[1]) == "guided, 4":
                    guided_time = guided_time + (float("%.3f" % float(data[3])),)
                if str(data[1]) == "dynamic, 16":
                    dynamic_time = dynamic_time + (float("%.3f" % float(data[3])),)
                if str(data[1]) == "affinity":
                    affinity_time = affinity_time + (float("%.3f" % float(data[3])),)
        i = i + 1

    if loop_no == 1:
        create_plots(
            "loop1_running_time",
            titles[0],
            "Running Time (sec)",
            ["guided, 4", "affinity"],
            [1, 2, 4, 6, 8, 12, 16],
            [guided_time, affinity_time],
            0.4,
            "upper right"
        )
    elif loop_no == 2:
        create_plots(
            "loop2_running_time",
            titles[0],
            "Running Time (sec)",
            ["dynamic, 16", "affinity"],
            [1, 2, 4, 6, 8, 12, 16],
            [dynamic_time, affinity_time],
            0.4,
            "upper right"
        )


def analyze_data_speedup(data_file, loop_no):

    i = 0;

    titles = ()
    schedule = ()
    affinity_speedup = ()
    guided_speedup = ()
    dynamic_speedup = ()
    affinity_time1 = 0
    guided_time1 = 0
    dynamic_time1 = 0
    threads = [1, 2, 4, 6, 8, 12, 16]
    ideal_speedup = (1, 2, 4, 6, 8, 12, 16)

    for line in data_file:
        data = line.split('\t')
        if i == 0:
            titles = data
        else:
            if loop_no == int(data[2]):
                schedule = schedule + (str(data[1]),)
                if str(data[1]) == "affinity":
                    if affinity_speedup == ():
                        affinity_time1 = float(data[3])
                    affinity_speedup = affinity_speedup + ( float("%.3f" % float(affinity_time1/float(data[3])))  ,)
                if str(data[1]) == "guided, 4":
                    if guided_speedup == ():
                        guided_time1 = float(data[3])
                    guided_speedup = guided_speedup + ( float("%.3f" % float(guided_time1/float(data[3])))  ,)
                if str(data[1]) == "dynamic, 16":
                    if dynamic_speedup == ():
                        dynamic_time1 = float(data[3])
                    dynamic_speedup = dynamic_speedup + ( float("%.3f" % float(dynamic_time1/float(data[3])))  ,)
        i = i + 1



    if loop_no == 1:
        create_plots(
            "loop1_speedup",
            titles[0],
            "Speedup",
            ["affinity", "guided, 4", "ideal"],
            threads,
            [affinity_speedup, guided_speedup, ideal_speedup],
            0.4,
            "lower right"
        )
    elif loop_no == 2:
        create_plots(
            "loop2_speedup",
            titles[0],
            "Speedup",
            ["affinity", "dynamic, 16", "ideal"],
            threads,
            [affinity_speedup, dynamic_speedup, ideal_speedup],
            0.4,
            "lower right"
        )


#create jmax graph
path = './data/jmax.tsv'
f = open(path, 'r')
analyze_jmax(f)

#parse results to create ./data/data_coursework2.tsv 
path = './data/results_coursework2.txt'
f = open(path, 'r')
parse_results_coursework2(f)

#parse ./data/data_coursework1.tsv and ./data/data_coursework2.tsv to create the merged ./data/results.tsv results
path1 = './data/data_coursework1.tsv'
path2 = './data/data_coursework2.tsv'
f1 = open(path1, 'r')
f2 = open(path2, 'r')
merge_coursework_results(f1, f2)

#analyze ./data/results.tsv to create the appropriate graphs
path = './data/results.tsv'

for loop in [1, 2]:
    f = open(path, 'r')
    analyze_data(f, loop)
    f = open(path, 'r')
    analyze_data_speedup(f, loop)
