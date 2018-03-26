
BDD preImage(Cudd mgr, DGame *game, BDD f_set_u, int player, BDD );
DGame *deleteGame(Cudd mgr, DGame *game, BDD f_remove);
BDD attractor(Cudd mgr,DGame *game,BDD f_u, int player, BDD);
DGame *ConvertMaxToMin(DGame *game, Cudd mgr);
BDD preImageApt(Cudd mgr, DGame *game, BDD f_set_u, int player, BDD f_removed);

