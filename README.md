# Node efficiency

This C++ program calculates the **efficiency** (https://en.wikipedia.org/wiki/Efficiency_(network_science)) of each node of the graph generated from an .edgelist file.
The list of efficiencies is saved to a .eff file, with the same prefix of the .edgelist file.

This program is an assignment for the "SFI5822 - Introdução à Programação Paralela" (Introduction to Parallel Programming) course, tought by professor Gonzalo Travieso at the University of São Paulo (2020).

Since the studied graph type is: unweighted, undirected and sparse, the best suited algorithm to find all shortest paths is the **breadth first search (BFS)** applied to every node: http://en.wikipedia.org/wiki/Breadth-first_search.

To execute the BFS, it is more convenient and efficient to have the list of adjacent nodes for every node (instead of having to loop over the sparse adjacency matrix or the badly structured edge list), therefore we use the adjacency list to store the graph data.

The ```node_eff.cpp``` program and the ```node_eff_no_list``` program achieve the same results.
The ```node_eff.cpp``` is the first implementation of the code, which the queue in the BFS is implemented with the ```list``` object.
The ```node_eff_no_list.cpp``` uses a ```vector``` as queue, achieving roughly a 2 times improviment in calculation speeds.

## Compilation

I reccomend to compile using:
```
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 node_eff_no_list.cpp -o node_eff_no_list
```

## Usage
You can use the compiled program to get the efficiency of and individual graph by:
```
./node_eff_no_list ba_n_1000_k_10_0_example.edgelist
```

If you want to process multiple files, save them to a directory named ```test-data-eff``` in the same location of the ```bach_process.py``` program and run it with:
```
python3 bach_process.py
```
The .eff files will be outputted to the ```test-data-eff```directory.
