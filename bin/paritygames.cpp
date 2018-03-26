 #include <iostream>
#include <cstdlib>
#include <ctime>
#include <util.h>
#include <cudd.h>
#include <cuddObj.hh>
#include <cmath>
#include "basic.h"
#include "paritygames.h"
#include "generator.h"
using namespace std;

BDD preImageApt(Cudd mgr, DGame *game, BDD f_set_u, int player, BDD f_removed){

    BDD tmp,app,f_and,exist,forall,f_or;
    BDD f_pl,f_opp,pre_image,var;

    int n_var= game->n_var;
    int i;

    vector<BDD> var_f(n_var);
    vector<BDD> var_prime(n_var);

    BDD cube=mgr.bddOne();
    
    int j=0;

    for(i=n_var;i<(2*n_var);i++){

        var=mgr.bddVar(i);
        var_f[j]=mgr.bddVar(j);
        var_prime[j]=var;
        j++;
        tmp=var * cube;
        cube=tmp;
    }
   
    if(player == 0){
        f_pl=game->f_V0;
        f_opp=game->f_V1;
    }
    else{
        f_pl=game->f_V1;
        f_opp=game->f_V0;
    }
    
    BDD f_u=f_set_u.SwapVariables(var_f,var_prime); 

    f_and=f_u *  game->f_edges;
    //exist x' | f_u and f_edges
    exist=f_and.ExistAbstract(cube);
    pre_image=f_pl * exist ;

    f_or=!game->f_edges + f_u ;
    //for all x' | not f_edges or f_u
    forall=f_or.UnivAbstract(cube);
    //f_1-pl and forall
    pre_image=pre_image + (f_opp * forall);
    
    return pre_image;
}



BDD preImage(Cudd mgr, DGame *game, BDD f_set_u, int player, BDD f_removed){
   
    BDD tmp,app,f_and,exist,forall,f_or;
    BDD f_pl,f_opp,pre_image,var;

    int n_var= game->n_var;
    int i;

    vector<BDD> var_f(n_var);
    vector<BDD> var_prime(n_var);

    BDD cube=mgr.bddOne();
    
    int j=0;

    for(i=n_var;i<(2*n_var);i++){

        var=mgr.bddVar(i);
        var_f[j]=mgr.bddVar(j);
        var_prime[j]=var;
        j++;
        tmp=var * cube;
        cube=tmp;
    }
   
    BDD f_V0_rm = game->f_V0 * !f_removed;
    BDD f_V1_rm = game->f_V1 * !f_removed;


    if(player == 0){
        f_pl=f_V0_rm;
        f_opp=f_V1_rm;
    }
    else{
        f_pl=f_V1_rm;
        f_opp=f_V0_rm;
    }
    
    BDD f_u=f_set_u.SwapVariables(var_f,var_prime); 
    BDD f_rem_primed=f_removed.SwapVariables(var_f,var_prime);

    f_and=f_u *  game->f_edges;
    //exist x' | f_u and f_edges
    exist=f_and.ExistAbstract(cube);
    pre_image=f_pl * exist ;

    f_or=!game->f_edges + f_u + f_rem_primed;
    //for all x' | not f_edges or f_u
    forall=f_or.UnivAbstract(cube);
    //f_1-pl and forall
    pre_image=pre_image + (f_opp * forall);
    //pre_image=app;
    
    // remove from pre_image nodes are not in the game
    pre_image = pre_image * !f_removed;

    return pre_image;
}



DGame *deleteGame(Cudd mgr, DGame *game, BDD f_remove){
    
    BDD tmp,f_app,f_neg;
    BDD f_rem_add;
    ADD constant,bdd_add,temp,temp_or;
    int n_var= game->n_var;

    DGame *game_remove=new DGame;

    int max_prio=game->max_prio;
 
    tmp=game->f_V0 * !f_remove;
    game_remove->f_V0=tmp;

    tmp=game->f_V1 * !f_remove;
    game_remove->f_V1=tmp;

    vector<BDD> vars(n_var);
    vector<BDD> vars2(n_var);
    int i,y;

    for(i=0;i<n_var;i++){
        vars[i]=mgr.bddVar(i);   
        vars2[i]=mgr.bddVar(n_var+i);
    }

    BDD f_rem_copy=f_remove.SwapVariables(vars,vars2);

    //new edge function
    f_app=f_remove + f_rem_copy;
    tmp=game->f_edges * !f_app;
    game_remove->f_edges=tmp;
  

    //new priorty funciton
    game_remove->f_pr=vector<BDD> (max_prio+1);

    for(i=0;i<game->max_prio;i++){
        game_remove->f_pr[i]=game->f_pr[i] * !f_remove;
    }
 	game_remove->f_pr[game->max_prio]=game->f_pr[game->max_prio] + f_remove;

    ADD f_priority= mgr.constant(0);

    
    for(y=max_prio-1;y>=0;y--){
 		constant=mgr.constant(y);
        bdd_add=game_remove->f_pr[y].Add();
        temp=constant * bdd_add;

        temp_or=temp + f_priority;
        f_priority=temp_or;
    }
    
    constant=mgr.constant(game->max_prio);
    bdd_add=game_remove->f_pr[max_prio].Add();
    temp=constant * bdd_add;

    temp_or=temp + f_priority;

    game_remove->f_priority=temp_or;
    game_remove->max_prio=game->max_prio;
    game_remove->n_var=game->n_var;
 
    return game_remove;
}


BDD attractor(Cudd mgr,DGame *game,BDD f_u, int player, BDD f_removed){
    
    BDD f_iplus,f_i,f_int;
    
    f_i=mgr.bddOne();
    f_iplus=f_u;

    do{
        
        f_i=f_iplus;
        f_int=preImage(mgr,game,f_i,player,f_removed);

        f_iplus= f_i + f_int;        
    }while( f_iplus != f_i);

    return f_iplus;
}


DGame *ConvertMaxToMin(DGame *game, Cudd mgr){

  DGame *gmin= new DGame;
  int dim,j;


  gmin->N=game->N;
  gmin->n_var=game->n_var;

  gmin->n_nodes=game->n_nodes;

  gmin->f_V0=game->f_V0;
  gmin->f_V1=game->f_V1;
  gmin->f_edges=game->f_edges;

  gmin->f_priority=mgr.addZero();

  if(game->max_prio % 2==0 ){// ultima priorità dispari
    dim=game->max_prio+2;
    gmin->max_prio=game->max_prio+1;
    j=1;
  }
  else{
    gmin->max_prio=game->max_prio;
    dim=game->max_prio+1;
    j=0;
  }

  vector<BDD> f_pr(dim,mgr.bddZero());
  vector<int> n_nodes(gmin->max_prio,0);
  
  for(int i=game->max_prio-1;i>=0;i--){
    f_pr[j]=game->f_pr[i];
    n_nodes[j]=game->n_nodes[i];
    j++;
  }
  f_pr[gmin->max_prio]=game->f_pr[game->max_prio];
  gmin->f_pr=f_pr;
  
  gmin->n_nodes=n_nodes;

  return gmin;

}

