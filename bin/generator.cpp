#include <iostream>
#include <cstdlib>
//#include <ctime>
#include <util.h>
#include <cudd.h>
#include <cuddObj.hh>
#include <cmath>
#include "basic.h"
#include "generator.h"
using namespace std;

DGame *randomGame(Cudd mgr,int N, int max_prio, int min_edges, int max_edges, int dist){
    
    
    if(min_edges>max_edges){
        return NULL;
    }
    if(max_prio <= 0){//solo priorità
        return NULL;
    }
    
    FILE *fd;
    DGame *game=new DGame;
    int i,pr,pl,y,n_var2;
    int n_var=ceil(log2(N));
    game->n_var=n_var;

    vector<BDD> f_pr(max_prio+1,mgr.bddZero());
    vector<int> n_nodes(max_prio,0);

    vector<BDD> f_nodes(N);

    BDD f_V0,f_V1,f_edges,f_v,tmp,var,f_adj,f_bdd,var2;
    
    ADD f_priority,constant,bdd_add,f_app,temp;
    
    vector<int> v(n_var);
    vector<int> w(n_var);
    int j,x,*adj,num_edges,d;

    f_V0=mgr.bddZero();
    f_V1=mgr.bddZero();
    f_edges=mgr.bddZero();
    
    fd=fopen("test.pm", "w");
    if( fd==NULL ) {
        perror("Error,file not found");
        exit(1);
    }

    fprintf(fd,"parity %d;\n",N);

    for(y=0;y<=max_prio;y++){
        f_pr[y]=mgr.bddZero();
    }

    for(i=0;i<N;i++){
        f_v=mgr.bddOne();

        j=0;
        decimalToBinary(i,v,n_var);
        
        for(y=n_var-1;y>=0;y--){
            var=mgr.bddVar(y);
            if(v[j]==0)
                tmp= !var * f_v;
            else
                tmp= var * f_v;
            f_v=tmp;
            j++;
        }
        fprintf(fd,"%d ",i);

        pr=rand() % max_prio ;// 0,..,max_prio-1
        n_nodes[pr]++;
        fprintf(fd,"%d ",pr);

        pl=rand() % 2;
        fprintf(fd,"%d ",pl);

        if(pl==0){
            tmp=f_v + f_V0;
            f_V0=tmp;
        }
        else{
            
            tmp=f_v +f_V1;
            f_V1=tmp;
        }
        num_edges=random_num(min_edges,max_edges);
        
        if(num_edges > dist)
            num_edges=dist;
       

        adj = new int[num_edges];
        
        random_without_rip(num_edges,N,adj,dist,i);
        for(x=0;x<num_edges;x++){
            fprintf(fd,"%d",adj[x]);
         //   cout << adj[x] << "\n";
            
        
            if(x!=num_edges-1){
                fprintf(fd,",");
            }
            decimalToBinary(adj[x],w,n_var);
            j=0;
            
            f_adj=mgr.bddOne();
          
            n_var2=(n_var*2)-1;
            for(y=n_var-1;y>=0;y--){
                var=mgr.bddVar(y);
                if(v[j]==0)
                    tmp=!var * f_adj;
                else
                    tmp=var * f_adj;
               
                f_adj=tmp;
                var2=mgr.bddVar(n_var2);
                n_var2--;
                
                if(w[j]==0)
                    tmp=!var2 * f_adj;
                else
                    tmp=var2 * f_adj;
                
                f_adj=tmp;
                j++;
            }
            
            tmp=f_adj + f_edges;
            f_edges=tmp;
        }
        delete [] adj;
        
        fprintf(fd," \"%d\";\n",i);
        
        tmp=f_v + f_pr[pr];
        f_pr[pr]=tmp;
    }

    int pw=pow(2,n_var);

    if(N !=pw){
        
        for(i=N; i<pw;i++){
            
            j=0;
            decimalToBinary(i,v,n_var);
            
            f_bdd=mgr.bddOne();
            
            for(y=n_var-1;y>=0;y--){
                var=mgr.bddVar(y);
                
                if(v[j]==0)
                    tmp=!var * f_bdd;
                else
                    tmp=var * f_bdd;
                
                f_bdd=tmp;
                j++;
            }
            
            tmp=f_bdd + f_pr[max_prio];
            f_pr[max_prio]=tmp;
        }
    }
    
    f_priority=mgr.constant(0);
   
    
    for(y=max_prio-1;y>=0;y--){
        constant=mgr.constant(y);
        bdd_add=f_pr[y].Add();
        f_app=constant * bdd_add;
        temp=f_app + f_priority ;
        f_priority=temp;
    }

    constant=mgr.constant(max_prio);
    bdd_add=f_pr[max_prio].Add();
    f_app=constant * bdd_add;
    
    temp=f_app + f_priority;    
    f_priority=temp;
		
    game->N=N;
    game->f_V = f_V0 + f_V1;
    game->f_V0=f_V0;
    game->f_V1=f_V1;
    game->f_edges=f_edges;
    game->f_priority=f_priority;
    game->f_pr=f_pr;
    game->max_prio=max_prio;
    game->n_nodes=n_nodes;

    fclose(fd);

    return game;
}


DGame *readGame(Cudd mgr, char file[], int maxprio){
    
    
    char buffer[100];
    char string[10];
    char c;
    int p;
    int j,x,adj,num_edges,d;
    BDD f_V0,f_V1,f_edges,f_v,tmp,var,f_adj,f_bdd,var2;

    FILE *fd;
    
    fd=fopen(file, "r");
    if( fd==NULL ) {
        perror("error");
        exit(1);
    }  
    fread(buffer,sizeof(char),7,fd);
    p=0;
    do{
        fread(&c,sizeof(char),1,fd);
        string[p]=c;
        p++;
    }while(c!=';');  
    string[p-1]='\0';
    int N=atoi(string);


    int max_prio=maxprio;
    fread(buffer,sizeof(char),2,fd);

    DGame *game=new DGame;
    int i,pr,pl,y,n_var2;
    int n_var=ceil(log2(N));
    game->n_var=n_var;


    vector<BDD> f_pr(max_prio+1,mgr.bddZero());
    unordered_map<int, BDD > f_prio;
    unordered_map<int, BDD > n_nods;

    vector<int> n_nodes(max_prio,0);

    BDD var_nodes[n_var];
    BDD var_edges[2*n_var];
    
    ADD f_priority,constant,bdd_add,f_app,temp;
    
    vector<int> v(n_var);
    vector<int> w(n_var);
    
    f_V0=mgr.bddZero();  
    f_V1=mgr.bddZero();  
    f_edges=mgr.bddZero();
   
    for(y=0;y<=max_prio;y++){
        f_pr[y]=mgr.bddZero();
    }
    
    for(i=0;i<N;i++){
        f_v=mgr.bddOne();  

        j=0;
        decimalToBinary(i,v,n_var);
        
        for(y=n_var-1;y>=0;y--){
            var=mgr.bddVar(y);
            if(v[j]==0)
                tmp= !var * f_v;
            else
                tmp= var * f_v;
            f_v=tmp;
            j++;
        }
        do{
            fread(&c,sizeof(char),1,fd);
        }while(c!=' ');
        p=0;
        do{
            fread(&c,sizeof(char),1,fd);
            string[p]=c;
            p++;
        }while(c!=' ');  
        string[p-1]='\0';
        pr=atoi(string);
        n_nodes[pr]++;

        fread(string,sizeof(char),1,fd);//pl
        string[1]='\0';
        pl=atoi(string); 
      
        fread(buffer,sizeof(char),1,fd);

        if(pl==0){
            tmp=f_v + f_V0;
            f_V0=tmp;
        }
        else{
            
            tmp=f_v +f_V1;
            f_V1=tmp;
        }

        char prev;

        while(c!=';'){

            p=0;
            do{
                prev=c;
                fread(&c,sizeof(char),1,fd);
                string[p]=c;
                p++;
            }while(c!=',' && c!=' ' && c!=';' && c!=34);  
            string[p-1]='\0';

            if(c==' ' && prev==',') continue;
            if (c==34){// character "
                do{ fread(&c,sizeof(char),1,fd); }
                while(c!=34);
                c=';';
                continue;
            }
            adj=atoi(string);

            decimalToBinary(adj,w,n_var);
            j=0;

         
            f_adj=mgr.bddOne();

            n_var2=(n_var*2)-1;
            for(y=n_var-1;y>=0;y--){
                var=mgr.bddVar(y);
                if(v[j]==0)
                    tmp=!var * f_adj;
                else
                    tmp=var * f_adj;
               
                f_adj=tmp;
                var2=mgr.bddVar(n_var2);
                n_var2--;
                if(w[j]==0)
                    tmp=!var2 * f_adj;
                else
                    tmp=var2 * f_adj;
                
                f_adj=tmp;
                j++;
            }
            
            tmp=f_adj + f_edges;
            f_edges=tmp;
        }

   
        tmp=f_v + f_pr[pr];
        f_pr[pr]=tmp;
    }

    int pw=pow(2,n_var);

    if(N !=pw){
        
        for(i=N; i<pw;i++){
            
            j=0;
            decimalToBinary(i,v,n_var);
            
            f_bdd=mgr.bddOne();
            
            for(y=n_var-1;y>=0;y--){
                var=mgr.bddVar(y);
                
                if(v[j]==0)
                    tmp=!var * f_bdd;
                else
                    tmp=var * f_bdd;
                
                f_bdd=tmp;
                j++;
            }
            
            tmp=f_bdd + f_pr[max_prio];
            f_pr[max_prio]=tmp;
        }
    }
    
    f_priority=mgr.constant(0);
   
    
    for(y=max_prio;y>=0;y--){
        constant=mgr.constant(y);
        bdd_add=f_pr[y].Add();
        f_app=constant * bdd_add;
        temp=f_app + f_priority ;
        f_priority=temp;
    }
    
    
    constant=mgr.constant(max_prio);
    bdd_add=f_pr[max_prio].Add();
    f_app=constant * bdd_add;
    
    temp=f_app + f_priority;    
    f_priority=temp;
        
    game->N=N;
    game->f_V = f_V0 + f_V1;
    game->f_V0=f_V0;
    game->f_V1=f_V1;
    game->f_edges=f_edges;
    game->f_priority=f_priority;
    game->f_pr=f_pr;
    game->max_prio=max_prio;
    game->n_nodes=n_nodes;
    
    fclose(fd);

    return game;
}

void decimalToBinary(int n, int binary[], int size) {
    int i=0;
    int j;
    
    while (n) {
        binary[i] = n % 2;
        n = n / 2;
        i++;
    }
    for (j=i;j<size;j++){
        binary[j]=0;
    }
}


void decimalToBinary(int n, vector<int> &binary, int size) {
    int i=0;
    int j;
    
    while (n) {
        binary[i] = n % 2;
        n = n / 2;
        i++;
    }
    for (j=i;j<size;j++){
        binary[j]=0;
    }

}

int random_num(int a, int b)
{ /* genera un numero casuale tre a e b */
    int ret = a+(rand()%(b-a+1));
    return (ret);
}

void random_without_rip(int num,int N, int numbers[], int dist, int nodo){
    
    int *value=new int[N];
    int i,v,d;
    int cont=0;

    for(i=0;i<N;i++){
        value[i]=-1;
    }

    while(cont!=num){
        v=rand()%N;
        d=abs(nodo-v);
        
        if(d <= dist){      
            
            if(value[v]!=v){
               
                value[v]=v;
                numbers[cont]=v;
                cont++;
            }
        }
    }
    delete [] value;
}


void computePriority(char file[]){
	char buffer[100];
    char string[100];
    char c;
    int p,pr;

    int max=-1;
	FILE *fd;
    
    fd=fopen(file, "r");
    if( fd==NULL ) {
        perror("Errore in apertura del file");
        exit(1);
    }  


    fread(buffer,sizeof(char),7,fd);
    p=0;
    do{
        fread(&c,sizeof(char),1,fd);
        string[p]=c;
        p++;
    }while(c!=';');
    string[p-1]='\0';
            cout<<"sono qui \n";

    int N=atoi(string);
     cout<<"sono qui "<<N<<"\n";
    int i=0;
    while(i < N){
	    do{
	        fread(&c,sizeof(char),1,fd);
	    }while(c!=' '); 
	    p=0;
	    do{
	        fread(&c,sizeof(char),1,fd);
	        string[p]=c;
	        p++;
	    }while(c!=' ');
	    string[p-1]='\0';
	    pr=atoi(string);

	    cout<<"sono qui "<<pr<<"\n";
	    if( pr>max)
	    	max=pr;

	    do{
	        fread(&c,sizeof(char),1,fd);
	    }while(c!=';'); 

	    i++;
	}

	cout<<"massima priorità "<<max<<"\n";

	    fclose(fd);
}

void print_nodes(Cudd mgr,DGame *game, BDD f){
    
    
    int val[game->n_var],y,p,j,i;
    double test;
    BDD eval;
    ADD bdd_add;
    vector<int> v(game->n_var);

    for(i=0;i<pow(2,game->n_var);i++){
        
        decimalToBinary(i,v,game->n_var);
              p=0;
        for(j=game->n_var-1;j>=0;j--){


            val[j]=v[p];
            p++;
        }
        eval= f.Eval(val);
        
       test=Cudd_V(eval.Add().getNode());
      
        if(test){
            cout<<"nodo "<<i<<"\n";
        }
        
        }
}

