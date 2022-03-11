#!/bin/python3

from numpy import array_split, abs
import subprocess
import sys
import pathlib
import pygad

constant_dimensions = (8, 8)

# PG_AI constants writer
def write_constant_file(constant: list):
    constant = array_split(constant, constant_dimensions[0])

    c_file = open('PG_AI/constant.h', 'w+')

    data = f"const unsigned int priorities[{len(constant)}][{len(constant[0])}] = {{\n"
    for i, line in enumerate(constant):
        data += "  {"
        for j, val in enumerate(line):
            data += str(abs(val))
            if j < len(constant[0])-1:
                data += ', '
        data += '}'
        if i < len(constant)-1:
            data += ', '
        data += '\n'
    data += '};\n'

    c_file.write(data)

def f_readline(filename) -> str:
    with open(filename) as f:
        for line in f.readlines():
            yield line

def for_files(path: pathlib.Path) -> pathlib.Path:
    for f in path.iterdir():
        if f.is_file():
            yield f

def calculate_avg(file: pathlib.Path):
    avg = 0
    count = 0
    for i, line in enumerate(f_readline(file)):
        if i < 6:
            continue

        if len(line) > 10:
            count += 1
            line = line.split("\t")
            packet_creation = int(line[7])
            trailer_cycle = int(line[9])
            avg += trailer_cycle - packet_creation

    return avg, count

# reads snocs generated files and calculate the average packet delay
def calculate_delay_avg() -> float:
    avg = 0
    count = 0
    handlers = {}
    for path in ["results/"]:
        path = pathlib.Path(path).absolute()
        handlers[path] = [f for f in for_files(path)]

    for path, files in handlers.items():
        for file in files:
            tavg, tcount = calculate_avg(file)
            count += tcount
            avg += tavg
    avg /= tcount
    return avg

# genetic algorithm function
def fitness_func(solution, solution_idx):
    write_constant_file(solution)
    subprocess.call("./make_and_sim.sh", shell=False)
    output = calculate_delay_avg()
    fitness = 1.0 / abs(output)
    print(solution_idx, solution)
    print(f"avg: {output} | fitness: {fitness}")
    return fitness

# genetic algorithm parameters
num_generations = 50
num_parents_mating = 4

sol_per_pop = 8
num_genes = constant_dimensions[0] * constant_dimensions[1]

init_range_low = 0
init_range_high = 8

parent_selection_type = "sss"
keep_parents = 1

crossover_type = "single_point"

mutation_type = "random"
mutation_percent_genes = 10

# create pygad instance
ga_instance = pygad.GA(
    num_generations=num_generations,
    num_parents_mating=num_parents_mating,
    fitness_func=fitness_func,
    sol_per_pop=sol_per_pop,
    num_genes=num_genes,
    gene_type=int,
    init_range_low=init_range_low,
    init_range_high=init_range_high,
    parent_selection_type=parent_selection_type,
    keep_parents=keep_parents,
    crossover_type=crossover_type,
    mutation_type=mutation_type,
    mutation_percent_genes=mutation_percent_genes
)
ga_instance.run()

solution, solution_fitness, solution_idx = ga_instance.best_solution()
print("Parameters of the best solution : {solution}".format(solution=solution))
print("Fitness value of the best solution = {solution_fitness}".format(solution_fitness=solution_fitness))

constant = array_split(solution, constant_dimensions[0])
print('Best solution constants:')
for i, line in enumerate(constant):
    for j, val in enumerate(line):
        print(abs(val), end=', ')
    print('')



