#include <unordered_map>
#include <cstdio>

using namespace std;


struct Game{
	int N;
    int n_var;
    BDD f_V;
    BDD f_V0;
    BDD f_V1;
    BDD f_edges;
    ADD f_priority;
    vector<BDD> f_pr;//array
    int max_prio;
    vector<int> n_nodes;
};

struct Winning{
    BDD win_0;
    BDD win_1;
};

struct InfoGame{
    int max_prio;
    int N;
    int n_var;
    vector<int> n_nodes;
    unordered_map<double, vector<double> > tuple;
};
typedef struct InfoGame IGame;
typedef struct Game DGame;
typedef struct Winning DWinning;


