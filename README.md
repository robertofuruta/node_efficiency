# Node efficiency

This C++ program calculates the **efficiency** (https://en.wikipedia.org/wiki/Efficiency_(network_science)) of each node of the graph generated from an .edgelist file.
The list of efficiencies is saved to a .eff file, with the same prefix of the .edgelist file.

This program is an assignment for the "SFI5822 - Introdução à Programação Paralela" (Introduction to Parallel Programming) course, tought by professor Gonzalo Travieso at the University of São Paulo (2020).

Since the studied graph type is: unweighted, undirected and sparse, the best suited algorithm to find all shortest paths is the **breadth first search (BFS)** applied to every node: http://en.wikipedia.org/wiki/Breadth-first_search.

To execute the BFS, it is more convenient and efficient to have the list of adjacent nodes for every node (instead of having to loop over the sparse adjacency matrix or the badly structured edge list), therefore we use the adjacency list to store the graph data.

The ```node_eff_sequential_list.cpp``` program and the ```node_eff_sequential_array.cpp``` program achieve the same results.
The ```node_eff_sequential_list.cpp``` is the first implementation of the code, in which the queue for the BFS is implemented with the ```list``` object.
The ```node_eff_sequential_array.cpp``` uses a ```vector``` as queue, although less intuitive, achieves roughly a 2 times improvement in calculation speeds.

The ```node_eff_parallel_array_openmp.cpp``` is a parallel implementation of the code above, using the OpenMP for C++.

The ```node_eff_parallel_array_mpi.cpp``` is a parallel implementation of the code above, using the OpenMPI for C (C++).


## Compilation

I recommend to compile using:
```
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 node_eff_sequential_array.cpp -o node_eff_sequential_array
```
For the OpenMP parallel program, I recommend to compile using:
```
g++ -std=c++17 -O2 -fopenmp node_eff_parallel_array_openmp.cpp -o node_eff_parallel_array_openmp
```
And for the MPI parallel program, I recommend to compile using:
```
mpic++ -std=c++17 -O2 node_eff_parallel_array_mpi.cpp -o node_eff_parallel_array_mpi
```

## Usage
You can use the compiled program to get the efficiency of and individual graph by:
```
./node_eff_sequential_array ba_n_1000_k_10_0_example.edgelist
```

for the OpenMP parallel program, you need to inform the number of threads as well:
```
./node_eff_sequential_array ba_n_1000_k_10_0_example.edgelist 4
```
and for tho MPI parallel program, you execute it a bit differently
```
mpirun -np 4 node_eff_parallel_array_mpi ba_n_1000_k_10_0_example.edgelist
```

If you want to process multiple files, save them to a directory named as you wish and run:

```
find . -name '*.edgelist' -exec  YOUR COMMAND HERE {} \;
```
for example:

```
find . -name '*.edgelist' -exec node_eff_sequential_array {} \;
```

```
find . -name '*.edgelist' -exec node_eff_parallel_array_openmp {} 4 \;
```

```
find . -name '*.edgelist' -exec mpirun -np 4 /node_eff_parallel_array_mpi {} \;
```
The .eff files will be outputted to the same directory in which the .edgelist is found.

