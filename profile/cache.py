#!/bin/python

import pandas
import sys
from pprint import pprint
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print("Error: please provide a path to a results json file to plot")
    sys.exit(1)

results = pandas.read_json(sys.argv[1])

grouped = results.groupby("size")

for size in grouped.groups.keys():
    group = grouped.get_group(size)
    plt.plot(group["bytes"], group["total"], label=f"file_size={size}")

plt.yscale("log")
plt.xscale("log", base=2)
plt.xticks(results["bytes"].unique())
plt.legend()
plt.xlabel("bytes read")
plt.ylabel("seek+read time (ns)")
plt.title("investigating file system caching via pointer chasing")

plt.show()

# plt.show()