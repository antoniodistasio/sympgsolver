//#include "basic.h"
void computePriority(char file[]);

DGame *randomGame(Cudd mgr,int N, int max_prio, int min_edges, int max_edges,int dist);
void random_without_rip(int num,int N ,int[], int dist,int nodo);
int random_num(int a, int b);
void decimalToBinary(int , vector<int> &, int);
DGame *readGame(Cudd mgr, char [],int);
void print_nodes(Cudd mgr,DGame *game, BDD f);
bool read_uint64(std::streambuf *rd, uint64_t *res);
void skip_whitespace(std::streambuf *rd);
DGame * parse_pgsolver(Cudd, istream &inp);


