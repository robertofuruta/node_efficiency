//This prigram calculates the efficiency of each node of the graph generated from the .edgelist file.
//The list of efficiencies is saved to a .eff file, with the same prefix of the .edgelist file.

#include <fstream>
#include <vector>
#include <list>
#include <iostream>
#include <sstream>
using namespace std;

bool breadth_first_search(vector<int> adj_list[], int src, int N, int dist[])
{
    list<int> queue;            // list of nodes not evaluated yet
    bool visited[N];            // each position represents each node, it stores if the node was visited at least once
    for (int i = 0; i < N; i++) //initializing each array
    {
        visited[i] = false;
        dist[i] = INT_MAX;
        // pred[i] = -1;
    }

    // updating the first node: src
    visited[src] = true;
    dist[src] = 0;
    queue.push_back(src);

    // standard BFS algorithm
    while (!queue.empty())
    {                             //while there are nodes to evaluate
        int node = queue.front(); //the first node is chosen
        queue.pop_front();        //and it is removed from the queue

        for (int i = 0; i < adj_list[node].size(); i++)
        { //for each index of neighbour
            if (visited[adj_list[node][i]] == false)
            {                                             // if it's not been visited:
                visited[adj_list[node][i]] = true;        // update to visited
                dist[adj_list[node][i]] = dist[node] + 1; // +1 step distance
                // pred[adj_list[node][i]] = node;           // register the predecessor
                queue.push_back(adj_list[node][i]); // add to the queue
            }
        }
    }

    return false; // if was't able to arrive at the destination.
}

int main(int argc, char *argv[])
{
    if (argc != 2) //Reads the program name and the .edgelist file name from the terminal
    {
        cerr
            << "Give in the command line a file name of the edge list.\n";
        return 1;
    }

    ifstream file(argv[1]); //Checks if the .edgelist file is accessible
    if (!file.is_open())
    {
        cerr << "Error opening file " << argv[1] << std::endl;
        return 2;
    }

    //if there is no error,
    //the adjacency list (vector of vector of int) will be created
    //first the number of edges is needed
    int N; //the number of edges is read from the .edgelist file name "???_n_'N'_k_'?_?'.edgelist"
    vector<string> split_file_name;
    stringstream s_stream(argv[1]); //string stream object
    while (s_stream.good())
    {
        string substr;
        getline(s_stream, substr, '_'); //splits the filename at "_"
        split_file_name.push_back(substr);
    }

    N = stoi(split_file_name[2]); //the N is the [2]th element

    //now let's create the vector and read the file.
    vector<int> adj_list[N];

    string line;
    while (getline(file, line)) //for each line in the edgelist file,
    {
        int src, dest;
        istringstream ss(line);
        ss >> src >> dest;             //the src (source) and dest (destination) node pair (edge) is read
        adj_list[src].push_back(dest); //append the neighbourt to the list of adjacent nodes
        adj_list[dest].push_back(src); //append the neighbourt to the list of adjacent nodes
    };

    // debug: print each neighbour pair
    // for (int i = 0; i < N; i++)
    //     for (int j = 0; j < adj_list[i].size(); j++)
    //         cout << "From node " << i << " to node " << adj_list[i][j] << endl;

    int dist_mat[N][N]; //stores the distances from src
    for (int i = 0; i < N; i++)
    {
        breadth_first_search(adj_list, i, N, dist_mat[i]); //gets the distance from node i to all the nodes
    }

    double eff_list[N]; // stores the efficiency of each node
    for (int i = 0; i < N; i++)
    {
        cout << "node eff " << i << endl;
        eff_list[i] = 0; //initializes as 0.
        for (int j = 0; j < N; j++)
        {
            if (i != j && dist_mat[i][j] != INT_MAX) // if not the source node itself, neither isolated:
            {
                eff_list[i] += 1. / dist_mat[i][j] / (N - 1);
            }
        }
        cout << eff_list[i] << endl;
    }
}
