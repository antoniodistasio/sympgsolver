#include <iostream>
#include <cstdlib>
#include <util.h>
#include <cudd.h>
#include <cuddObj.hh>
#include <cmath>
#include "run.h"
#include "basic.h"
#include "generator.h"
#include "paritygames.h"
#include "solvers.h"
#include "benchmark.h"

#include "run.h"

using namespace std;


void inputRandom(Cudd manager,int argc, char *argv[]){
      
      int n_var=ceil(log2(atoi(argv[3]))); 
      int N=atoi(argv[3]);
      DGame *game;

      if(strcmp(argv[2],"RE")==0){
         
        int solution[N],solution_apt[N],v[n_var],val[n_var],p,test;
        ADD eval;

        game=randomGame(manager,atoi(argv[3]),atoi(argv[4]),1,atoi(argv[3]),atoi(argv[5]));
        DWinning *win=recursiveAlgorithm(manager,game);

        for(int i=0;i<N;i++){
          decimalToBinary(i,v,n_var);//codifica nodo i in binario

          p=0;
          for(int j=n_var-1;j>=0;j--){
            val[j]=v[p];
            p++;
          }
            
          eval= (win->win_0.Add()).Eval(val);

          test=Cudd_V(eval.getNode());
          if(test)
            solution[i]=0;
          else
            solution[i]=1;
        }
 
        cout<<"W0: ";

        for(int i=0;i<N;i++){
          if(solution[i]==0)
          cout<<" "<<i;
        }
        cout<<"\n";
        cout<<"W1: ";

        for(int i=0;i<N;i++){
          if(solution[i]==1)
          cout<<" "<<i;
        }
        cout<<"\n";

      }

      if(strcmp(argv[2],"SSPM")==0){

        int solution[N],v[n_var],val[n_var],p,test;
        ADD eval2;
        
        game=randomGame(manager,atoi(argv[3]),atoi(argv[4]),1,atoi(argv[3]),atoi(argv[5]));
        DGame *gmin=ConvertMaxToMin(game,manager);
        BDD winSPM = setBasedSPM(manager,gmin);

        for(int i=0;i<N;i++){
          decimalToBinary(i,v,n_var);//codifica nodo i in binario

          p=0;
          for(int j=n_var-1;j>=0;j--){
            val[j]=v[p];
            p++;
          }
            
          eval2= (winSPM.Add()).Eval(val);
          test=Cudd_V(eval2.getNode());
          if(test)
              solution[i]=0;
          else
              solution[i]=1;
        }
 
        cout<<"W0: ";

        for(int i=0;i<N;i++){
          if(solution[i]==0)
          cout<<" "<<i;
        }
        cout<<"\n";
        cout<<"W1: ";

        for(int i=0;i<N;i++){
          if(solution[i]==1)
          cout<<" "<<i;
        }
        cout<<"\n";

      }
      if(strcmp(argv[2],"APT")==0){

        int solution[N],v[n_var],val[n_var],p,test;
        ADD eval,eval2,eval3;

        game=randomGame(manager,atoi(argv[3]),atoi(argv[4]),1,atoi(argv[3]),atoi(argv[5]));
        BDD win_1=aptSolver(manager,game);
        for(int i=0;i<N;i++){
          decimalToBinary(i,v,n_var);//codifica nodo i in binario

          p=0;
          for(int j=n_var-1;j>=0;j--){
            val[j]=v[p];
            p++;
          }
            
          eval= (win_1.Add()).Eval(val);

          test=Cudd_V(eval.getNode());
          if(test)
            solution[i]=1;
          else
            solution[i]=0;
        }
 
        cout<<"W0: ";

        for(int i=0;i<N;i++){
          if(solution[i]==0)
          cout<<" "<<i;
        }
        cout<<"\n";
        cout<<"W1: ";

        for(int i=0;i<N;i++){
          if(solution[i]==1)
          cout<<" "<<i;
        }
        cout<<"\n";

      }
      if(strcmp(argv[2],"SPM")==0){
         

        int solution[N],solution_apt[N],v[n_var],val[n_var],p;
        double test;
        ADD eval,eval2,eval3;

        game=randomGame(manager,atoi(argv[3]),atoi(argv[4]),1,atoi(argv[3]),atoi(argv[5]));
        ADD f_win=smpAlgorithm(manager,game);
        for(int i=0;i<N;i++){
          decimalToBinary(i,v,n_var);//codifica nodo i in binario

          p=0;
          for(int j=n_var-1;j>=0;j--){
            val[j]=v[p];
            p++;
          }
            
          eval= (f_win).Eval(val);

          test=Cudd_V(eval.getNode());
           
          if(test != INFINITY )
            solution[i]=0;
          else
            solution[i]=1;
        }
 
        cout<<"W0: ";

        for(int i=0;i<N;i++){
          if(solution[i]==0)
          cout<<" "<<i;
        }
        cout<<"\n";
        cout<<"W1: ";

        for(int i=0;i<N;i++){
          if(solution[i]==1)
          cout<<" "<<i;
        }
        cout<<"\n";

      }
  //  }

}

void inputBenchmark(Cudd manager,int argc, char *argv[]){
		int algorithms[4]={0};

    	if(strcmp(argv[2],"random")==0){
          
          int N=atoi(argv[3]);//N
          int maxprio= atoi(argv[4]);//maxprio
          int dist= atoi(argv[5]);//dist
          int n_exec= atoi(argv[6]);//n_exe

          for(int i=7; i< argc;i++){
            if(strcmp(argv[i],"RE") == 0)
              algorithms[0]=1;
            if(strcmp(argv[i],"APT") == 0)
              algorithms[1]=1;
            if(strcmp(argv[i],"SPM") == 0)
              algorithms[2]=1;
            if(strcmp(argv[i],"SSPM") == 0)
              algorithms[3]=1;
          }
          benchmark(manager,N,maxprio,1,N,dist,n_exec,algorithms);
        }


        if(strcmp(argv[2],"file")==0){
          int algorithms[4]={0};
          
          int maxprio= atoi(argv[4]);//maxprio
         
          int n_exec= atoi(argv[5]);//n_exe

          for(int i=6; i< argc;i++){
            if(strcmp(argv[i],"RE") == 0)
              algorithms[0]=1;
            if(strcmp(argv[i],"APT") == 0)
              algorithms[1]=1;
            if(strcmp(argv[i],"SPM") == 0)
              algorithms[2]=1;
            if(strcmp(argv[i],"SSPM") == 0)
              algorithms[3]=1;
          }
          benchmarkFromFile(manager,maxprio,n_exec,algorithms,argv[3]);

    	 }
 	//}

}