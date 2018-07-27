#include <iostream>
#include <cstdlib>
#include <ctime>
#include <util.h>
#include <cudd.h>
#include <cuddObj.hh>
#include <cmath>
#include "basic.h"
#include "generator.h"
#include "paritygames.h"
#include "solvers.h"
#include "tuple.h"
using namespace std;


DWinning *recursiveAlgorithm(Cudd mgr, DGame *game, BDD f_removed, int k){
    
    BDD f_win0,f_win1,D,zero,f_u;
    BDD f_rm;
    BDD x_pl,x_opp,w_pl,w_opp,b,f_union,y_pl,y_opp;
    DWinning *setswin,*xwin,*ywin;
    
    ADD bdd_add2,bdd_add;
    int pl;
    
    BDD f_ris= game->f_V * !f_removed;
    if(f_ris.IsZero() ){ // empty game
        setswin=new DWinning;
        setswin->win_0=mgr.bddZero();
        setswin->win_1=mgr.bddZero();
        return setswin;
    }
    
    setswin= new DWinning;
   
   int min_prio;
   BDD rim;
    for (int i=0; i < game->max_prio;i++){
        rim=game->f_pr[i] * !f_removed;
        if(!rim.IsZero()){
            min_prio=i;
            break;
        }
    }
    pl = min_prio % 2;
    
    f_u=game->f_pr[min_prio] * !f_removed;
    // set U \ Removed
    BDD attr = attractor(mgr,game,f_u,pl,f_removed);
 
    f_rm = f_removed + attr;
   
    xwin= recursiveAlgorithm(mgr,game,f_rm,k+1);//check
 
    if(pl==0){
        x_pl=xwin->win_0;
        x_opp=xwin->win_1;
    }
    else{
        x_pl=xwin->win_1;
        x_opp=xwin->win_0;
    }
    
    if(x_opp.IsZero())
    {
        w_pl=attr + x_pl;
        w_opp=mgr.bddZero();
    }
    else{
         
   
        b=attractor(mgr,game,x_opp,1-pl,f_removed);
        
        BDD f_rm2 = f_removed + b;
        ywin=recursiveAlgorithm(mgr,game,f_rm2,k+1);
        
        if(pl==0){
            y_pl=ywin->win_0;
            y_opp=ywin->win_1;
        }
        else{
            y_pl=ywin->win_1;
            y_opp=ywin->win_0;
        }
        w_pl=y_pl;
        w_opp=b + y_opp;
    }
    
    if(pl==0){
        setswin->win_0=w_pl;
        setswin->win_1=w_opp;
    }
    else{
        setswin->win_0=w_opp;
        setswin->win_1=w_pl;
    }
    
    return setswin;
}


BDD aptSolver(Cudd mgr, DGame *game){

    BDD visiting,avoiding,f_N;

    f_N=game->f_V0 + game->f_V1;
    visiting= mgr.bddZero();
    avoiding=mgr.bddZero();
    BDD win= aptAlgorithm(mgr,game,f_N,visiting,avoiding,0,1);
    
    return win;
}

BDD aptAlgorithm(Cudd mgr, DGame *game, BDD f_N, BDD visiting, BDD avoiding, int k, int pl){
   
    if(k <= game->max_prio){
        return min_fixed_point(mgr,game,f_N,visiting,avoiding,k+1,pl);
    }
    else
        return preImageApt(mgr,game,visiting,pl,mgr.bddZero());
}

BDD min_fixed_point(Cudd mgr, DGame *game, BDD f_N, BDD visiting, BDD avoiding, int k, int pl){

    BDD vs,av,f_i,f_iplus,f_F,apt,f_min,f_and,tmp;
    BDD av_old,vs_old;
    f_F=game->f_pr[k-1];
       
    f_iplus=mgr.bddZero();
    f_i=mgr.bddZero();
    av_old=vs_old=mgr.bddZero();
    do{
        f_and=f_F * f_iplus;
        vs=visiting + f_and;

        f_min=f_F * !f_iplus;
        av=avoiding + f_min;
        f_i=f_iplus;

       /* if(av == av_old && vs==vs_old && game->n_nodes[k-1])
            break;*/
        av_old=av;
        vs_old=vs;

        apt=aptAlgorithm(mgr,game,f_N,av,vs,k,1-pl);

        f_iplus=f_N * !apt;
    }while(f_iplus != f_i);

    return f_iplus;
}

ADD smpAlgorithm(Cudd mgr,DGame *game){

    ADD f_old,f,f_suc_min,f_suc_max,fd1,fd2,f_suc;
    int j;
    
    unordered_map<double, vector<double> > tuple;
    tuple.insert(make_pair(-INFINITY, vector<double> (1,-INFINITY)));//-infinity
    tuple.insert(make_pair(0, vector<double> (1,game->max_prio)));//undefined
    tuple.insert(make_pair(+INFINITY, vector<double> (1,INFINITY)));

    IGame *inf=new IGame;
    inf->N=game->N;
    inf->n_var=game->n_var;
    inf->tuple=tuple;
    inf->max_prio=game->max_prio;
    inf->n_nodes=game->n_nodes;

    BDD f_V= game->f_V0 + game->f_V1;
    f=f_V.Add() * mgr.constant(-INFINITY);;

    do{
        f_old=f;
        f=mgr.addZero();
              
        for(j=0;j<game->max_prio;j++){
        	f_suc_min=buildSuccessorFunction(mgr,game,f_old,0,j);
            f_suc_max=buildSuccessorFunction(mgr,game,f_old,1,j);
            fd1=MAXeo(mgr,f_suc_max,j,inf);
            fd2=MINeo(mgr,f_suc_min,j,inf);

            f= f + fd1 +fd2;
        }      
    }while(f_old != f);

    return f;
}
ADD Min (Cudd mgr, ADD f_suc, IGame *inf){

	if(Cudd_IsConstant(f_suc.getNode()))
		return f_suc;

	int index = f_suc.NodeReadIndex();
	ADD var=mgr.addVar(index);
	
    if(index < inf->n_var && index >=0)
        return var.Ite(Min(mgr,ADD(mgr,Cudd_T(f_suc.getNode())),inf),Min(mgr,ADD(mgr,Cudd_E(f_suc.getNode())),inf));
    else
        return MERGEmin(mgr,Min(mgr,ADD(mgr,Cudd_T(f_suc.getNode())),inf),Min(mgr,ADD(mgr,Cudd_E(f_suc.getNode())),inf),inf);

}

ADD MAXeo(Cudd mgr, ADD f_suc, int j, IGame *inf){

    ADD f_max;

    f_max=Max(mgr,f_suc,inf);
    if(j%2==0)
        return cutTuple(mgr,f_max,j,inf);
    else{
        return INC(mgr,f_max,j,inf);
    }

}

ADD MINeo(Cudd mgr, ADD f_suc, int j, IGame *inf){

    ADD f_min;

    f_min=Min(mgr,f_suc,inf);
    if(j%2==0)
        return cutTuple(mgr,f_min,j,inf);
    else{
        return INC(mgr,f_min,j,inf);
    }

}


ADD cutTuple (Cudd mgr, ADD f, int j, IGame *inf){

    double key;

    if(Cudd_IsConstant(f.getNode()))
    {   
        double v1 = Cudd_V(f.getNode());
        unordered_map<double,vector<double> >::const_iterator got;
        got = inf->tuple.find(v1);
        vector<double> d1= got->second;
        vector<double> t((j/2)+1);

        for(int i=0; i<=j/2;i++)
            t[i]=d1[i];

        key=associateKey(t,inf->N);
        inf->tuple.insert(make_pair(key,t));
        
        return mgr.constant(key);
    }
    else{
        int index = f.NodeReadIndex();
        ADD var=mgr.addVar(index);
        return var.Ite(cutTuple(mgr,ADD(mgr,Cudd_T(f.getNode())),j,inf),cutTuple(mgr,ADD(mgr,Cudd_E(f.getNode())),j,inf));
    }
}

ADD INC (Cudd mgr, ADD f, int j, IGame *inf){

    double key;

    if(Cudd_IsConstant(f.getNode()))
    {   
        double v1 = Cudd_V(f.getNode());
        unordered_map<double,vector<double> >::const_iterator got;
        got = inf->tuple.find(v1);
        vector<double> d1= got->second;
        d1 = inc(d1,ceil(j/2.0),j,inf->n_nodes);
        key=associateKey(d1,inf->N);
        inf->tuple.insert(make_pair(key,d1));

        return mgr.constant(key);
    }
    else{
        int index = f.NodeReadIndex();
        ADD var=mgr.addVar(index);
        return var.Ite(INC(mgr,ADD(mgr,Cudd_T(f.getNode())),j,inf),INC(mgr,ADD(mgr,Cudd_E(f.getNode())),j,inf));
    }
}
ADD Max (Cudd mgr, ADD f_suc, IGame *inf){

	if(Cudd_IsConstant(f_suc.getNode()))
    {	
      return f_suc;
    }
	int index = f_suc.NodeReadIndex();
	ADD var=mgr.addVar(index);

	if(index < inf->n_var && index >=0)
		return var.Ite(Max(mgr,ADD(mgr,Cudd_T(f_suc.getNode())),inf),Max(mgr,ADD(mgr,Cudd_E(f_suc.getNode())),inf)  );
	else
		return MERGEmax(mgr,Max(mgr,ADD(mgr,Cudd_T(f_suc.getNode())),inf),Max(mgr,ADD(mgr,Cudd_E(f_suc.getNode())),inf),inf);

}


ADD MERGEmax(Cudd mgr, ADD n1, ADD n2, IGame *inf ){

	ADD var;
	int index_n1,index_n2;
    double key;
	
	index_n1=n1.NodeReadIndex();
	index_n2=n2.NodeReadIndex();
	if(Cudd_IsConstant(n1.getNode()) && Cudd_IsConstant(n2.getNode()))
        return maxNode(n1,n2,inf);

	if(mgr.ReadPerm(index_n1) < mgr.ReadPerm(index_n2)){
		var=mgr.addVar(index_n1);
		return var.Ite(MERGEmax(mgr,ADD(mgr,Cudd_T(n1.getNode())),n2,inf)  , MERGEmax(mgr,ADD(mgr,Cudd_E(n1.getNode())),n2,inf)  );
	}
	else{
		var=mgr.addVar(index_n2);
        return var.Ite(MERGEmax(mgr,ADD(mgr,Cudd_T(n2.getNode())),n1,inf) , MERGEmax(mgr,ADD(mgr,Cudd_E(n2.getNode())),n1,inf));
	}
	var=mgr.addVar(index_n1);
	return var.Ite( MERGEmax(mgr,ADD(mgr,Cudd_T(n1.getNode())),ADD(mgr,Cudd_T(n2.getNode())),inf) ,MERGEmax(mgr,ADD(mgr,Cudd_E(n1.getNode())),ADD(mgr,Cudd_E(n2.getNode())),inf)  );

}


ADD MERGEmin(Cudd mgr, ADD n1, ADD n2, IGame *inf ){

	ADD var;
	int index_n1,index_n2;
    double key;

	index_n1=n1.NodeReadIndex();
	index_n2=n2.NodeReadIndex();

	if(Cudd_IsConstant(n1.getNode()) && Cudd_IsConstant(n2.getNode()))
		return minNode(n1,n2,inf);

	if(mgr.ReadPerm(index_n1) < mgr.ReadPerm(index_n2)){
		var=mgr.addVar(index_n1);
		return var.Ite(MERGEmin(mgr,ADD(mgr,Cudd_T(n1.getNode())),n2,inf),MERGEmin(mgr,ADD(mgr,Cudd_E(n1.getNode())),n2,inf));
	}
	else{
		var=mgr.addVar(index_n2);
		return var.Ite(MERGEmin(mgr,ADD(mgr,Cudd_T(n2.getNode())),n1,inf),MERGEmin(mgr,ADD(mgr,Cudd_E(n2.getNode())),n1,inf));
	}
	var=mgr.addVar(index_n1);
	return var.Ite(MERGEmin(mgr,ADD(mgr,Cudd_T(n1.getNode())),ADD(mgr,Cudd_T(n2.getNode())),inf),MERGEmin(mgr,ADD(mgr,Cudd_E(n1.getNode())),ADD(mgr,Cudd_E(n2.getNode())),inf));
}

ADD minNode(ADD n1, ADD n2, IGame *inf){

	double v1 = Cudd_V(n1.getNode());
	double v2 = Cudd_V(n2.getNode());

    if((v1 == INFINITY && v2== 0)|| (v1==0 && v2==INFINITY)){
        if(v1==INFINITY)
            return n1;
        else
            return n2;
    }

	unordered_map<double,vector<double> >::const_iterator got;
	got = inf->tuple.find(v1);
	vector<double> d1= got->second;
	got = inf->tuple.find(v2);
	vector<double> d2= got->second;

	int max;
	if(d1.size() > d2.size())
		max=d1.size();
	else
		max=d2.size();

	if(compareTuple(d1,d2,max) < 0) //d1 < d2
		return n1;
	else
		return n2;
}

ADD maxNode(ADD n1, ADD n2, IGame *inf){

	double v1 = Cudd_V(n1.getNode());
	double v2 = Cudd_V(n2.getNode());

    if(v1 == INFINITY || v2 == INFINITY){
        if(v1==INFINITY)
            return n1;
        else
            return n2;
    }

    if(v1 == 0 || v2 == 0){
        if(v1 == 0)
            return n2;
        else
            return n1;
    }

	unordered_map<double,vector<double> >::const_iterator got;
	got = inf->tuple.find(v1);
	vector<double> d1= got->second;
	got = inf->tuple.find(v2);
	vector<double> d2= got->second;

	int max;
	if(d1.size() > d2.size())
		max=d1.size();
	else
		max=d2.size();

	if(compareTuple(d1,d2,max) > 0) //d1 > d2
		return n1;
	else
		return n2;
}



ADD buildSuccessorFunction(Cudd mgr, DGame *game, ADD f, int pl, int pr){
    
    BDD f_Ej;
    ADD f_suc,f_prime,var;
    int i,j;

    vector<ADD> var_f(game->n_var);
    vector<ADD> var_prime(game->n_var);
    j=0;

    for(i=game->n_var;i<(2*game->n_var);i++){

        var=mgr.addVar(i);
        var_f[j]=mgr.addVar(j);
        var_prime[j]=var;
        j++;  
    }
    f_prime=f.SwapVariables(var_f,var_prime);

    if(pl==0){
    	f_Ej=game->f_V0 * game->f_pr[pr] * game->f_edges;
    }
    else{
    	f_Ej=game->f_V1 * game->f_pr[pr] * game->f_edges; 
    }
    f_suc = f_Ej.Add() * f_prime;

    return f_suc;
}




BDD setBasedSPM(Cudd mgr, DGame *game){

    BDD f_V, win_0,set_sr,set_sr_dec,cpre_o,f_union,f_i,set_T,set_sr_prime,subset,set_sr_old;

    unordered_map<int, vector<BDD> > sets;
    vector<double> r(game->max_prio,0);
    vector<double> r_prime(game->max_prio,0);
    int x=game->max_prio,l;

    f_V = game->f_V0 + game->f_V1;


    //initialitation map S_0= V
    for(int i=1;i<game->max_prio;i+=2){
       vector<BDD> set_c (game->n_nodes[i]+1,mgr.bddZero());
       set_c[0]=f_V;
       sets.insert(make_pair(i,set_c));
    }

    vector<BDD> set_c (1,mgr.bddZero());
    sets.insert(make_pair(game->max_prio,set_c));

    set_sr=mgr.bddZero();
    subset=mgr.bddZero();
    set_sr_prime=mgr.bddZero();
    set_T=mgr.bddZero();

    r = inc_tuple(r,0,game->n_nodes);
    while(1){
       
        if(r.size()!=1){

            set_sr=compute_Sr(mgr, sets,r,game->max_prio,game->n_nodes);
            set_sr_old=set_sr;

            //let l be maximal such that r = <r>l;
            for(int i=1;i<x;i+=2){
                if(r[i]!=0){
                    l=i;
                    break;
                }
            }

            f_union=mgr.bddZero();

            for(int k=1;k<=(l+1)/2;k++){//sr=sr U (U 1<=k<=(l+1)) Cpreo

                r_prime=dec_tuple(r,2*k-1,game->n_nodes);
                set_sr_dec=compute_Sr(mgr, sets,r_prime,game->max_prio,game->n_nodes);
                cpre_o=preImage(mgr,game,set_sr_dec,1, mgr.bddZero());
                f_union =f_union + (cpre_o * game->f_pr[2*k-1]);
            }
            set_sr+=f_union;

            // fixed point for S_r
            f_i=mgr.bddZero();  
            f_union=mgr.bddZero();
            for(int k=l+1; k<game->max_prio;k++){
                f_union+=game->f_pr[k];
            }         
            do{
                
                f_i=set_sr;

                cpre_o=preImage(mgr,game,set_sr,1,mgr.bddZero());               
                set_sr = set_sr + (cpre_o - f_union);      
               
            }while(set_sr!= f_i);

        }
        else{// r=T

            set_T=compute_Sr(mgr, sets,r,game->max_prio,game->n_nodes);
            set_sr_old=set_T;

            r_prime=r;

            f_union=mgr.bddZero();
            for(int k=1;k<=floor(game->max_prio/2);k++){
                r_prime=dec_tuple(r,2*k-1,game->n_nodes);
                set_sr_dec=compute_Sr(mgr, sets,r_prime,game->max_prio,game->n_nodes);
                cpre_o=preImage(mgr,game,set_sr_dec,1,mgr.bddZero());
                f_union =f_union + (cpre_o * game->f_pr[2*k-1]);
            }
            set_T+=f_union;

            f_i=mgr.bddZero();           
            do{
                
                f_i=set_T;

                cpre_o=preImage(mgr,game,set_T,1,mgr.bddZero());
                set_T +=cpre_o;      
               
            }while(set_T!= f_i);   

            set_sr=set_T;      
        }

        r_prime=dec_tuple(r,0,game->n_nodes);

        set_sr_prime=compute_Sr(mgr, sets,r_prime,game->max_prio,game->n_nodes);

        subset= set_sr - (set_sr_prime * set_sr);// check if S_r is a subset of S_r'
    
        if(subset.IsZero() && r.size()!=1){

            
            sets=update_Sr(mgr,set_sr_old,set_sr,sets,game->max_prio,game->n_nodes,r);


             r=inc_tuple(r,0,game->n_nodes);


        }
        else if(subset.IsZero() &&  r.size()==1 ){
                sets=update_Sr(mgr,set_sr_old,set_sr,sets,game->max_prio,game->n_nodes,r); 
            break;
        }
        else{
            do{
                r_prime=dec_tuple(r_prime,0,game->n_nodes); 
           
                set_sr_prime=compute_Sr(mgr, sets,r_prime,game->max_prio,game->n_nodes);
                subset= set_sr - (set_sr_prime * set_sr);
                
            }while(!(subset.IsZero()));

            sets=update_Sr(mgr,set_sr_old,set_sr,sets,game->max_prio,game->n_nodes,r);
            r=inc_tuple(r_prime,0,game->n_nodes);

        }

    }
    unordered_map<int,vector<BDD> >::const_iterator got;
    // set T
    got = sets.find(game->max_prio);


    return (f_V - (got->second)[0]);
}

BDD compute_Sr(Cudd mgr, unordered_map<int, vector<BDD> > sets, vector<double> r, int max_prio, vector<int> n_nodes){

    BDD set_sr_T,set_eq,set_sr_0,set_sr_i,set_sr,inter,uni;

    set_eq=mgr.bddOne();
    set_sr_0=mgr.bddZero();

    int c= max_prio;
    if(c%2)// odd
        c--;

    unordered_map<int,vector<BDD> >::const_iterator got;
    // set T
    got = sets.find(max_prio);
    vector<BDD> set_c= got->second;
    set_sr_T=set_c[0];

    if(r.size()==1)
        return set_sr_T;


    for(int k =1;k<=floor(max_prio/2);k++){
        got= sets.find(2*k-1);
        set_c=got->second;
        set_eq *= set_c[r[2*k-1]];
    }

    got= sets.find(c-1);
    set_c=got->second;
    for(int x=r[c-1]+1; x<=n_nodes[c-1];x++){
         set_sr_0 += set_c[x];
    }

    set_sr_i=mgr.bddZero();
    for(int i=1 ;i<=floor(max_prio/2)-1; i++){
                  
        inter=mgr.bddOne();

        for(int k=1; k<=i;k++){

            got= sets.find(c-(2*k)+1); 
            set_c=got->second;
           
            inter *= set_c[r[c-(2*k)+1]];

        }

        uni=mgr.bddZero();  
        got= sets.find(c-(2*i)-1);
        set_c=got->second;  
        for(int x=r[c-(2*i)-1]+1; x<=n_nodes[c-(2*i)-1];x++){
            uni += set_c[x];

        }

        set_sr_i+=inter * uni;
    }

    set_sr=set_sr_T + set_eq + set_sr_i+ set_sr_0;

    return set_sr;
}

unordered_map<int,vector<BDD> > update_Sr (Cudd mgr, BDD set_sr_old, BDD set_sr_new, unordered_map<int, vector<BDD> > sets, int c,vector<int> n_nodes, vector<double> r){

    BDD set_sr_diff;
    vector<BDD> set_c;

    set_sr_diff = set_sr_new - set_sr_old;

    unordered_map<int,vector<BDD> >::const_iterator got;

    for(int i=1;i<c;i+=2){
        got= sets.find(i);
        set_c=got->second;

        for(int x =0 ;x<=n_nodes[i];x++)
            set_c[x] = set_c[x] - set_sr_diff;

        sets[i]=set_c;

    }

    if(r.size() == 1 ){
        got= sets.find(n_nodes.size());
        set_c= got->second;

        set_c[0]+= set_sr_diff;
        sets[n_nodes.size()]=set_c;

    }
    else{

        for(int i=1;i<c;i+=2){
            got= sets.find(i);
            set_c=got->second;

            set_c[r[i]] +=set_sr_diff;

            sets[i]=set_c;
        }
    }
    return sets;
}


