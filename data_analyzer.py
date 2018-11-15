#!/usr/bin/env python2
# data_analyzer.py

import os
import matplotlib.pyplot as plt


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
    ideal_speedup = ()
    affinity_time1 = 0
    ideal_time1 = 0

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
                if str(data[1]) == "ideal":
                    if ideal_speedup == ():
                        ideal_time1 = float(data[3])
                    ideal_speedup = ideal_speedup + ( float("%.3f" % float(ideal_time1/float(data[3])))  ,)
        i = i + 1

    if loop_no == 1:
        create_plots(
            "loop1_speedup",
            titles[0],
            "Speedup",
            ["affinity", "ideal"],
            [1, 2, 4, 6, 8, 12, 16],
            [affinity_speedup, ideal_speedup],
            0.4,
            "lower right"
        )
    elif loop_no == 2:
        create_plots(
            "loop2_speedup",
            titles[0],
            "Speedup",
            ["affinity", "ideal"],
            [1, 2, 4, 6, 8, 12, 16],
            [affinity_speedup, ideal_speedup],
            0.4,
            "lower right"
        )


path = './data/results.tsv'

if os.path.exists(path):
    f = open(path, 'r')
    analyze_data(f, 1)
    f = open(path, 'r')
    analyze_data(f, 2)
    f = open(path, 'r')
    analyze_data_speedup(f, 1)
    f = open(path, 'r')
    analyze_data_speedup(f, 2)
