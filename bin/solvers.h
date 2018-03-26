
DWinning *recursiveAlgorithm(Cudd mgr, DGame *game, BDD , int );
BDD aptSolver(Cudd mgr, DGame *game);
BDD aptAlgorithm(Cudd mgr, DGame *game, BDD f_N, BDD visiting, BDD avoiding, int k, int pl);
BDD min_fixed_point(Cudd mgr, DGame *game, BDD f_N, BDD visiting, BDD avoiding, int k, int pl);
ADD smpAlgorithm(Cudd mgr,DGame *game);
ADD Min (Cudd mgr, ADD f_suc, IGame *inf);
ADD MINeo(Cudd mgr, ADD f_suc, int j, IGame *inf);
ADD INC (Cudd mgr, ADD f, int j, IGame *inf);
ADD cutTuple (Cudd mgr, ADD f, int j, IGame *inf);
ADD Max (Cudd mgr, ADD f_suc, IGame *inf);
ADD MERGEmax(Cudd mgr, ADD n1, ADD n2, IGame *inf );
ADD MERGEmin(Cudd mgr, ADD n1, ADD n2, IGame *inf );
ADD minNode(ADD n1, ADD n2, IGame *inf);
ADD maxNode(ADD n1, ADD n2, IGame *inf);
ADD buildSuccessorFunction(Cudd mgr, DGame *game, ADD f, int pl, int pr);
ADD MAXeo(Cudd mgr, ADD f_suc, int j, IGame *inf);


BDD compute_Sr(Cudd mgr, unordered_map<int, vector<BDD> > sets, vector<double> r, int c, vector<int> n_nodes);
unordered_map<int,vector<BDD> > update_Sr (Cudd mgr, BDD set_sr_old, BDD set_sr_new,unordered_map<int, vector<BDD> > sets, int c,vector<int> n_nodes, vector<double> r);
BDD setBasedSPM(Cudd mgr, DGame *game);
