#!/bin/python3

import matplotlib.pyplot as plt

fl = open("evolution.log", "r")

generations = []
pop = []
for line in fl.readlines()[1:]:
    splitted = line.split(" ")
    idx = int(splitted[1])
    avg = float(splitted[4])
    fit = float(splitted[7])
    res = [idx, avg, fit]
    if len(pop) == 0:
        pop = [res]
    elif idx < pop[-1][0]:
        generations.append(pop)
        pop = [res]
    else:
        pop.append(res)
generations.append(pop)
fl.close()

evolution = []
for i, pop in enumerate(generations):
    vals = [res[1] for res in pop]
    evolution.append(vals)

plt.figure()
plt.title("Genetic Algorithm Evolution to Improve Arbiter Priority Generation")
plt.xlabel("Generation")
plt.ylabel("Average number of cycles")
plt.grid()
plt.boxplot(evolution, positions=range(len(evolution)))
plt.show()
