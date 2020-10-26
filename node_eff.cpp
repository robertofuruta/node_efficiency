//This prigram calculates the efficiency of each node of the graph generated from the .edgelist file.
//The list of efficiencies is saved to a .eff file, with the same prefix of the .edgelist file.

#include <fstream>
#include <vector>
#include <list>
#include <iostream>
#include <sstream>
using namespace std;

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
    for (int i = 0; i < N; i++)
        for (int j = 0; j < adj_list[i].size(); j++)
            cout << "From node " << i << " to node " << adj_list[i][j] << endl;
}
