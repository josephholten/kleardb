#!/bin/python

import pandas
import sys
from pprint import pprint
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print("Error: please provide a path to a results json file to plot")
    sys.exit(1)

results = pandas.read_json(sys.argv[1])

sizes = results["block_size"].to_numpy()

def plot(fieldx, fieldy):
    plt.plot(results[fieldx], results[fieldy], label=fieldy)

plot("block_size", "avg_latency")
plot("block_size", "max_latency")
plot("block_size", "throughput")

plt.yscale("log")
plt.xscale("log", base=2)
plt.xticks(sizes)
plt.legend()
plt.title("read() performance for varying block sizes")

plt.show()