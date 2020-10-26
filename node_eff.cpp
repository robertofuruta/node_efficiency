// Calcula a eficiência de cada nó do grafo a partir de um arquivo .edgelist (lista de arestas)
// e salva em um arquivo de mesmo nome, mas com sufixo .eff

#include <fstream>
#include <vector>
#include <list>
#include <iostream>
#include <sstream>
using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2) //primeiro lê-se as entradas na linha de comando.
    {
        cerr
            << "Give in the command line a file name of the edge list.\n";
        return 1;
    }

    ifstream file(argv[1]); //o argumento 1 será o nome do arquivo .edgelist, verifica-se se é possível acessá-lo
    if (!file.is_open())
    {
        cerr << "Error opening file " << argv[1] << std::endl;
        return 2;
    }

    //se não houve erro, vamos construir o vetor de adjacência
    //precisamos primeiro da quantidade de nós
    int N; //quantidade de nós, pelo nome do arquivo (mesmo quando não há conexões, considera-se que o nó existe e está isolado)
    vector<string> split_file_name;
    stringstream s_stream(argv[1]); //criando objeto string stream
    while (s_stream.good())
    {
        string substr;
        getline(s_stream, substr, '_'); //quebra a string em todos os "_"
        split_file_name.push_back(substr);
    }

    N = stoi(split_file_name[2]); //o termo [2] refere-se à quantidade de nós

    //agora vamos construir a lista de adjacencia
    //ele será um vector (nós) de vector de int (vizinhos)
    vector<int> adj_list[N];
    string line;
    list<int> node_list; //primeiro, quero verificar se há nós isolados (que não têm vizinhos.)
    while (getline(file, line))
    {
        int src, dest;
        istringstream ss(line);
        ss >> src >> dest;
        adj_list[src].push_back(dest);
        adj_list[dest].push_back(src);
    };
    for (int i = 0; i < N; i++)
        for (int j = 0; j < adj_list[i].size(); j++)
            cout << "From node " << i << " to node " << adj_list[i][j] << endl;
}
