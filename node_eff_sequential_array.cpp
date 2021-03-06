/*
* @author Roberto Hiroshi Matos Furuta
* Contact: roberto.furuta@usp.br

* This program calculates the efficiency (https://en.wikipedia.org/wiki/Efficiency_(network_science))
* of each node of the graph generated from the .edgelist file.
* The list of efficiencies is saved to a .eff file, with the same prefix of the .edgelist file.

* This program is an assignment for the "SFI5822 - Introdução à Programação Paralela"
* (Introduction to Parallel Programming) course, tought by professor Gonzalo Travieso at the University of São Paulo (2020).

* Since the studied graph type is: unweighted, undirected and sparse, the best suited algorithm to find all shortest paths is
* the breadth first search (BFS) applied to every node: http://en.wikipedia.org/wiki/Breadth-first_search.

* To execute the BFS, it is more convenient and efficient to have the list of adjacent nodes for every node (instead of
* having to loop over the sparse adjacency matrix or the badly structured edge list), therefore we use the
* adjacency list to store the graph data.
*/

#include <climits> //to use INT_MAX
#include <vector>
#include <list>
#include <string>
#include <sstream>  // for string stream
#include <iostream> // for data mannagement
#include <fstream>  // for file mannagement
#include <iomanip>  // for setting the precision
#include <chrono>   // for monitoring the elapsed time

using namespace std;

// adj_list is used as a constant reference to avoid copying
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

    while (pos < next_empty) // while there are nodes in the queue which were evaluated yet
    {
        int node = queue[pos]; // the first node is chosen
        pos += 1;              // and it is removed from the queue

        for (unsigned int i = 0; i < adj_list[node].size(); i++) // for each index of neighbour
        // here we have spacial locality by accessing the adj_list[node] entries, allocated sequencially
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
    if (argc != 2) // Reads the program name and the .edgelist file name from the terminal
    {
        cerr
            << "Give in the command line a file name of the edge list.\n";
        return 1;
    }

    ifstream file(argv[1]); // Checks if the .edgelist file is accessible
    if (!file.is_open())
    {
        cerr << "Error opening file " << argv[1] << std::endl;
        return 2;
    }

    // if there is no error,
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

    // creating the .eff filename to use later
    string output_file;
    output_file = argv[1];
    size_t pos = output_file.find(".");
    output_file = output_file.substr(0, pos);
    output_file += ".eff";

    // the adjacency list (vector of vector of int) will be created
    vector<vector<int>> adj_list(N);

    string line;
    while (getline(file, line)) // for each line in the edgelist file,
    {
        int src, dest;
        istringstream ss(line);
        ss >> src >> dest;             // the src (source) and dest (destination) node pair (edge) is read
        adj_list[src].push_back(dest); // append the neighbour to the list of adjacent nodes
        adj_list[dest].push_back(src); // append the neighbour to the list of adjacent nodes
    };

    vector<double> eff_list(N, 0); // stores the efficiency of each node

    // for registering the time:
    cout << output_file << "'s time to determine the efficiency (in seconds) is:" << endl;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    for (int i = 0; i < N; i++) // for every node:
    {
        vector<int> dist_from_src(N, INT_MAX);               // stores the distances from src to node = index
        breadth_first_search(adj_list, i, N, dist_from_src); // gets the distance from node i to all the nodes

        for (int j = 0; j < N; j++) // for each neighbour:
        // here we have spacial locality between the distances allocated in the dist_from_src vector
        {
            if (i != j && dist_from_src[j] != INT_MAX)          // if it's not the source node itself, neither isolated:
            {                                                   // here we have temporal locality of accessing eff_list[i] several times.
                eff_list[i] += 1. / dist_from_src[j] / (N - 1); // increment the efficiency
            }
        }
    }
    // getting the duration:
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cout << elapsed_seconds.count() << endl;

    // after the eff_list is completely filled, it's content is written to the output_file
    ofstream myfile(output_file);
    for (int i = 0; i < N; i++)
    {
        myfile << std::setprecision(6) << std::fixed; // set 6 decimal places

        if (myfile.is_open())
        {
            myfile << eff_list[i] << endl;
        }
    }
    myfile.close();
}

/*
* Elapsed times:
* N = 10 between ~10^-6 s
* N = 1000 ~0.05 s
* N = 2000 ~0.2 s
* N = 5000 between ~2.0 s
*/