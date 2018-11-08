#!/usr/bin/env python2
# analyze_data.py

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter
import matplotlib.patches as mpatches

bin_str = 'BIN '
threads_str = 'THREADS '
loop1_time = 'Total time for 1000 reps of loop 1 = '
loop2_time = 'Total time for 1000 reps of loop 2 = '


def create_plots(title, x_axis_title, y_axis_title, labels, x_values, y_values, alpha, legent_pos):

    fig, ax = plt.subplots()
    ax.set_xlabel(x_axis_title)
    ax.set_ylabel(y_axis_title)

    # for x, y in zip(x_values, y_values):
    #     plt.text(0, y, " " + ("%.2f" % y))

    markers = ['go-', 'gs-', 'gD-']
    colors = ['#7293cb', '#84ba51', '#d35e60']

    lines = []

    i = 0
    for y_list in y_values:
        lines.append(plt.plot(x_values, y_list, markers[i], label=labels[i], linewidth=2, color=colors[i]))
        for value in y_list:
            plt.axhline(y=value, linewidth=1, color=colors[i], alpha=alpha, linestyle="--")
        i = i + 1

    # plt.legend(lines, labels)
    plt.legend(loc=legent_pos)

    fig.savefig('./graphs/' + str(title) + '.eps', format='eps', dpi=1000)
    plt.show()


def get_next_string(src, contains):
    if src.startswith(contains):
        return src[src.index(contains) + len(contains):len(src)-1]
    return None


def calculate(data_file, best_loop1_scheduler, best_loop2_scheduler):

    threads_list = ()
    loop1_speedup_list = ()
    loop2_speedup_list = ()

    scheduler = None

    for line in data_file:

        res = get_next_string(line, threads_str)
        if res is not None:
            threads = res
            threads_list = threads_list + (res,)
            continue

        res = get_next_string(line, bin_str)
        if res is not None:
            scheduler = res
            continue

        if scheduler == best_loop1_scheduler:
            res = get_next_string(line, loop1_time)
            if res is not None:
                loop1_speedup_list = loop1_speedup_list + (float("%.3f" % float(res)),)

        if scheduler == best_loop2_scheduler:
            res = get_next_string(line, loop2_time)
            if res is not None:
                loop2_speedup_list = loop2_speedup_list + (float("%.3f" % float(res)),)

    create_plots(
        "speedup_loop1",
        "Threads Number",
        "Speedup",
        ["guided, 4"],
        threads_list,
        [loop1_speedup_list],
        0.4,
        "lower right"
    )

    create_plots(
        "speedup_loop2",
        "Threads Number",
        "Speedup",
        ["dynamic, 16"],
        threads_list,
        [loop2_speedup_list],
        0.4,
        "lower right"
    )


def analyze_data(data_file, loop):

    threads_list = ()
    affinity_scheduling = ()
    guided04 = ()
    dynamic16 = ()

    scheduler = None

    for line in data_file:

        res = get_next_string(line, threads_str)
        if res is not None:
            threads = res
            threads_list = threads_list + (res,)
            continue

        res = get_next_string(line, bin_str)
        if res is not None:
            scheduler = res
            continue

        
        res = get_next_string(line, loop2_time)
        if res is not None and loop == "loop2":
            if scheduler == "./loops2":
                affinity_scheduling = affinity_scheduling + (float("%.3f" % float(res)),)
            elif scheduler == "./bin/loops_guided04":
                guided04 = guided04 + (float("%.3f" % float(res)),)
            elif scheduler == "./bin/loops_dynamic16":
                dynamic16 = dynamic16 + (float("%.3f" % float(res)),)

    print threads_list
    print affinity_scheduling
    print guided04
    print dynamic16

    create_plots(
        "running_time_loop2",
        "Threads Number",
        "Running Time",
        ["affinity_scheduling", "guided04", "dynamic16"],
        threads_list,
        [affinity_scheduling, guided04, dynamic16],
        0.4,
        "upper right"
    )


# f = open('./data/results_coursework1.dat', 'r')

# calculate(f, 'guided04', 'dynamic16')

f = open('./data/data.dat', 'r')

analyze_data(f, 'loop2')
