/*
* @author Roberto Hiroshi Matos Furuta
* Contact: roberto.furuta@usp.br

* This program calculates the efficiency (https://en.wikipedia.org/wiki/Efficiency_(network_science))
* of each node of the graph generated from the .edgelist file.
* The list of efficiencies is saved to a .eff file, with the same prefix of the .edgelist file.

* This program is an assignment for the "SFI5822 - Introdução à Programação Paralela"
* (Introduction to Parallel Programming) course, taught by professor Gonzalo Travieso at the University of São Paulo (2020).

* Since the studied graph type is: unweighted, undirected and sparse, the best suited algorithm to find all shortest paths is
* the breadth first search (BFS) applied to every node: http://en.wikipedia.org/wiki/Breadth-first_search.

* To execute the BFS, it is more convenient and efficient to have the list of adjacent nodes for every node (instead of
* having to loop over the sparse adjacency matrix or the badly structured edge list), therefore we use the
* adjacency list to store the graph data.

* The program is implemented using MPI for parallel processing.
*/

#include <climits> //to use INT_MAX
#include <vector>
#include <string>
#include <sstream>  // for string stream
#include <iostream> // for data mannagement
#include <fstream>  // for file mannagement
#include <iomanip>  // for setting the precision
#include <chrono>   // for monitoring the elapsed time
#include <mpi.h>    // for using mpi
#include <numeric>  // to be able to create "counts" and "displs"

using namespace std;

// adj_list is used as a constant reference to avoid copying

// this function is implemented equally to the sequential program
void breadth_first_search(const vector<vector<int>> &adj_list, int src, int N, vector<int> &dist)
{
    vector<int> queue(N); // vector of sequence of nodes to evaluate
    //before, the queue as implemented using a list object, which was easier,
    //but after changing to an implementation with vector, the code got approx. 2 times faster.
    int pos = 0;        // the position in queue of the next node to evaluate
    int next_empty = 1; // the position in the queue to which a new node will be added

    vector<bool> visited(N, false); // each position represents each node, it stores if the node was visited at least once

    // updating the first node: src
    visited[src] = true;
    dist[src] = 0;
    queue[0] = src;

    while (pos < next_empty) // while there are nodes in the queue which were not evaluated yet
    {
        int node = queue[pos]; // the first node is chosen
        pos += 1;              // and it is removed from the queue

        for (unsigned int i = 0; i < adj_list[node].size(); i++) // for each index of neighbour
        // here we have spacial locality by accessing the adj_list[node] entries, allocated sequentially
        {
            if (visited[adj_list[node][i]] == false) // if it's not been visited:
            {
                visited[adj_list[node][i]] = true;        // update to visited
                dist[adj_list[node][i]] = dist[node] + 1; // +1 step distance to the neighbour adj_list[node][i]
                queue[next_empty] = (adj_list[node][i]);  // add to the queue
                next_empty += 1;                          // update the next empty position
            }
        }
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv); // begin MPI code segment

    int N_proc, rank; //number of processes and rank of each process (+ communicator)

    MPI_Comm_size(MPI_COMM_WORLD, &N_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // reads the program name and the .edgelist file name from the terminal

    // checks if the number of arguments doesn't mach the expected amount
    if (argc != 2)
    {
        if (rank == 0)
        {
            cerr << "Give in the command line the .edgelist file name and the number of threads.\n";
        }
        MPI_Finalize(); // finalizes all processes
        return 1;
    }

    // checks if the .edgelist file is accessible
    ifstream file(argv[1]);
    if (!file.is_open())
    {
        if (rank == 0)
        {
            cerr << "Error opening file " << argv[1] << endl;
        }
        MPI_Finalize(); // finalizes all processes
        return 2;
    }

    // if there is no error so far
    // the number of nodes read.
    int N; // N is read from the .edgelist file name "???_n_'N'_k_'?_?'.edgelist"
    vector<string> split_file_name;
    stringstream s_stream(argv[1]); // string stream object
    while (s_stream.good())
    {
        string substr;
        getline(s_stream, substr, '_'); // splits the filename at "_"
        split_file_name.push_back(substr);
    }

    N = stoi(split_file_name[2]); // the N is the [2]th element

    string output_file; // file in which the efficiency list will be saved
    string output_time; // file in which the spent time will be saved
    if (rank == 0)
    {
        // creating the .eff filename to use later
        output_file = argv[1];
        size_t pos = output_file.find(".edgelist");
        output_file = output_file.substr(0, pos);
        output_time = output_file;
        output_file += ".eff";
        output_time += "_mpi_.time";
    }

    // the adjacency list (vector of vector of int) will be created
    // from the edge list.
    // the edge list is flattened to facilitate the broadcasting
    // doing it in one message, minimizing communication time
    vector<vector<int>> adj_list(N);
    vector<int> flat_edgelist;

    // reading the edge list from file:
    string line;
    int src, dest;

    if (rank == 0) // rank 0 (root) reads the edge list from file
    {
        while (getline(file, line)) // for each line in the edgelist file,
        {
            istringstream ss(line);
            ss >> src >> dest; // the src (source) and dest (destination) node pair (edge) is read
            flat_edgelist.push_back(src);
            flat_edgelist.push_back(dest);
        };
    }

    // the size of the flat_edgelist is identified and broadcasted from root (rank 0) to all
    int flat_edgelist_size;
    flat_edgelist_size = flat_edgelist.size();
    MPI_Bcast(&flat_edgelist_size, 1, MPI_INT, 0, MPI_COMM_WORLD); // the edgelist is broadcasted to all ranks

    // the content of the flat_edgelist is broadcasted from root to all
    flat_edgelist.resize(flat_edgelist_size);
    MPI_Bcast(&flat_edgelist[0], flat_edgelist.size(), MPI_INT, 0, MPI_COMM_WORLD);

    //this flat edgelist is converted to an adjacency list, in every process
    for (int i = 0; i < flat_edgelist.size() / 2; i++) // for every edge:
    {
        src = flat_edgelist[2 * i];
        dest = flat_edgelist[2 * i + 1];
        adj_list[src].push_back(dest); // append the neighbour to the list of adjacent nodes
        adj_list[dest].push_back(src); // append the neighbour to the list of adjacent nodes
    }

    vector<double> partial_eff_list; // stores the efficiency of each node

    // Distribution of elements per process.
    // Each process will be responsable for calculating the efficiency of
    // some nodes. The number of nodes will be distributed evenly and
    // the remeinder will be distributed to the first nodes.
    //// For example: if 11 nodes will be distributed between 4 processes,
    //// rank 0: nodes [0, 1, 2]
    //// rank 1: nodes [3, 4, 5]
    //// rank 2: nodes [6, 7, 8]
    //// rank 3: nodes [9, 10]

    vector<int> counts(N_proc), displs(N_proc);

    int q = N / N_proc;                             // quotient
    int r = N % N_proc;                             // remeinder
    auto first_q = fill_n(begin(counts), r, q + 1); // fill the r first positions of counts with (q+1)
    fill(first_q, end(counts), q);                  // fill the empty spaces of counts with (q)
    displs[0] = 0;
    partial_sum(begin(counts), end(counts) - 1, begin(displs) + 1); // fill displs with index which each proc should start

    chrono::time_point<chrono::system_clock> start, end;
    if (rank == 0)
    {
        // for registering the time:
        cout << output_file << "'s time to determine the efficiency (in seconds) is:" << endl;
        start = chrono::system_clock::now();
    }

    for (int i = displs[rank]; i < displs[rank] + counts[rank]; i++) // for every node:
    {
        vector<int> dist_from_src(N, INT_MAX);               // stores the distances from src to node = index
        breadth_first_search(adj_list, i, N, dist_from_src); // gets the distance from node i to all the nodes
        double eff(0);                                       // stores de efficiency of the node i

        for (int j = 0; j < N; j++) // for each neighbour:
        // here we have spacial locality between the distances allocated in the dist_from_src vector
        {
            if (i != j && dist_from_src[j] != INT_MAX)  // if it's not the source node itself, neither isolated:
            {                                           // here we have temporal locality of accessing eff_list[i] several times.
                eff += 1. / dist_from_src[j] / (N - 1); // increment the efficiency
            }
        }
        partial_eff_list.push_back(eff); // the value of eff is transferred to the global eff_list vector
    }

    // Gathering
    vector<double> global_eff_list(N); // stores the efficiency of all nodes
    MPI_Gatherv(&partial_eff_list[0], counts[rank], MPI_DOUBLE,
                &global_eff_list[0], &counts[0], &displs[0], MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Outputting
    if (rank == 0)
    {
        // getting the duration:
        end = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end - start;
        cout << elapsed_seconds.count() << endl;

        // saving time into file
        ofstream timefile(output_time);
        timefile << setprecision(6) << fixed; // set 6 decimal places
        if (timefile.is_open())
        {
            timefile << elapsed_seconds.count() << endl;
        }
        timefile.close();

        // after the eff_list is completely filled, it's content is written to the output_file
        ofstream myfile(output_file);
        for (int i = 0; i < N; i++)
        {
            myfile << setprecision(6) << fixed; // set 6 decimal places

            if (myfile.is_open())
            {
                myfile << global_eff_list[i] << endl;
            }
        }
        myfile.close();
    }
    MPI_Finalize();
    return 0;
}

/*
* Elapsed times:
* Comparing with the performance of the openmp program, both with 4 processes
* in a quad core cpu provided via server.
* In average, the MPI implementation took 1.95 times longer than the OpenMP counterpart.
* The performance difference was the largest in the graphs with ~ 200 000 edges, being ~ 2.3 times slower
* The performance was closest in the largest networks with ~ 1 000 000 edges, being ~ 1.2 times slower
*/