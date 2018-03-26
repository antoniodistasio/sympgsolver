#include <iostream>
#include <cstdlib>
#include <ctime>
#include <util.h>
#include <cudd.h>
#include <cuddObj.hh>
#include <cmath>
#include <sys/time.h>

#include "basic.h"
#include "generator.h"
#include "paritygames.h"

#include "solvers.h"
#include "benchmark.h"

using namespace std;

 double
wctime()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec + 1E-6 * time.tv_usec;
}

void benchmark(Cudd manager, int n , int maxprio, int min_edges, int max_edges, int dist, int n_exec, int flag[]){

	double avg_time_re=0, avg_time_apt=0, avg_time_spm=0,avg_time_setbasedspm=0;
  double time_spent_re=0,time_spent_apt=0,time_spent_spm=0,time_spent_setbasedspm=0;
  clock_t begin,end,begin2,end2,begin3,end3,begin4,end4;
  DGame * game,*gmin;
  int j;
  int n_var=ceil(log2(n));
 
  vector<DGame *> games(n_exec);

 cout<<"Creation ...\n";
  for(int i=0; i< n_exec; i++){
    games[i]=randomGame(manager,n,maxprio,1,n,dist);
  }
 
 cout<<"Benchmarks ...\n";
 
  if(flag[0]){
      for(int i=0; i<n_exec;i++){

    	   begin = clock();            
    	   recursiveAlgorithm(manager,games[i],manager.bddZero(),0);
    	   end = clock();
         time_spent_re = (double)(end - begin) / CLOCKS_PER_SEC;
         avg_time_re+=time_spent_re;

      }

  }
  if(flag[1]){
    for(int i=0; i<n_exec;i++){

    	   begin2 = clock();
    	   aptSolver(manager,games[i]);
    	   end2 = clock(); 
    	   time_spent_apt = (double)(end2 - begin2) / CLOCKS_PER_SEC;
         avg_time_apt+=time_spent_apt;

    }
  }
    
  if(flag[2]){
    for(int i=0; i<n_exec;i++){

    	   begin3 = clock();
    	   smpAlgorithm(manager,games[i]);
    	   end3 = clock(); 
    	   time_spent_spm = (double)(end3 - begin3) / CLOCKS_PER_SEC;
         avg_time_spm+=time_spent_spm;


    }
  }
  if(flag[3]){
    for(int i=0; i<n_exec;i++){
           begin4 = clock();
           gmin=ConvertMaxToMin(games[i],manager);
           setBasedSPM(manager,gmin);
           end4 = clock(); 
           time_spent_setbasedspm = (double)(end4 - begin4) / CLOCKS_PER_SEC;
           avg_time_setbasedspm+=time_spent_setbasedspm;
        }
  }        
 


	avg_time_re=avg_time_re/(double)n_exec;
	avg_time_apt=avg_time_apt/(double)n_exec;
	avg_time_spm=avg_time_spm/(double)n_exec;
  avg_time_setbasedspm = avg_time_setbasedspm/(double)n_exec;
  cout<<"\n";
  cout<<"--------------------------------------------------\n";
  cout<<" N. executions: "<<n_exec <<" -- "<<"Nodes:"<<n<<" -- Priorities: "<<maxprio<<"\n";
  cout<<"--------------------------------------------------\n";
  if(flag[0]) cout<<" AVG recursive     : "<<avg_time_re<<"\n";
  if(flag[1])cout<<" AVG APT           : "<<avg_time_apt<<"\n";
  if(flag[2])cout<<" AVG SPM           : "<<avg_time_spm<<"\n";
  if(flag[3])cout<<" AVG Set-Based SPM : "<<avg_time_setbasedspm<<"\n";
  cout<<"--------------------------------------------------\n";


}
void benchmarkFromFile(Cudd manager, int maxprio, int n_exec, int flag[], char file[]){

    double avg_time_re=0, avg_time_apt=0, avg_time_spm=0,avg_time_setbasedspm=0;
    double time_spent_re,time_spent_apt,time_spent_spm,time_spent_setbasedspm;
    clock_t begin,end,begin2,end2,begin3,end3,begin4,end4;
    DGame * game,*gmin;
    int j;
    cout<<"Parsing ...\n";

    game=readGame(manager,file,maxprio);
   
    cout<<"Benchmarks ...\n";
    
    cout<<"Recursive ... \n";
    if(flag[0]){

      for(int i=0; i<n_exec;i++){

        
              double begin = wctime();       
             recursiveAlgorithm(manager,game,manager.bddZero(),0);
             double end = wctime();
             time_spent_re = (end-begin);
             cout<<"time recursive "<<time_spent_re<<" \n";
             avg_time_re+=time_spent_re;
      }
    }
    cout<<"APT ... \n";

    if(flag[1]){
      for(int i=0; i<n_exec; i++){
           begin2 = clock();
           aptSolver(manager,game);
           end2 = clock(); 
           time_spent_apt = (double)(end2 - begin2) / CLOCKS_PER_SEC;
                        cout<<"time apt "<<time_spent_apt<<" \n";

            avg_time_apt+=time_spent_apt;

      }
    }
    cout<<"SPM ... \n";

    if(flag[2]){
      for(int i=0; i<n_exec; i++){
           begin3 = clock();
           smpAlgorithm(manager,game);
           end3 = clock(); 
           time_spent_spm = (double)(end3 - begin3) / CLOCKS_PER_SEC;
            cout<<"time SPM "<<time_spent_spm<<" \n";

          avg_time_spm+=time_spent_spm;

      }
    }
        cout<<"SPM2 ... \n";

    if(flag[3]){
       gmin=ConvertMaxToMin(game,manager);
      for(int i=0; i<n_exec; i++){
           begin4 = clock();
           setBasedSPM(manager,gmin);
           end4 = clock(); 
           time_spent_setbasedspm = (double)(end4 - begin4) / CLOCKS_PER_SEC;
                       cout<<"time SSPM "<<time_spent_setbasedspm<<" \n";

           avg_time_setbasedspm+=time_spent_setbasedspm;

        }
    }

    
    avg_time_re=avg_time_re/(double)n_exec;
    avg_time_apt=avg_time_apt/(double)n_exec;
    avg_time_spm=avg_time_spm/(double)n_exec;
    avg_time_setbasedspm=avg_time_setbasedspm/(double)n_exec;
    cout<<"\n ";
    cout<<"--------------------------------------------------\n";
    cout<<"   N. executions     "<<n_exec <<"                           \n";
    cout<<"--------------------------------------------------\n";
    if (flag[0]) cout<<" AVG recursive     : "<<avg_time_re<<"                    \n";
    if (flag[1])cout<<" AVG APT           : "<<avg_time_apt<<"                    \n";
    if (flag[2])cout<<" AVG SPM           : "<<avg_time_spm<<"                     \n";
    if (flag[3])cout<<" AVG Set-Based SPM : "<<avg_time_setbasedspm<<"                     \n";
    cout<<"--------------------------------------------------\n";


}

