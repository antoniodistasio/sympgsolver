#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <util.h>
#include <cudd.h>
#include <cuddObj.hh>
#include <cmath>
#include <string.h>
#include <sstream>
#include <unordered_map>
#include "basic.h"
#include "generator.h"
#include "paritygames.h"
#include "solvers.h"
#include "benchmark.h"
#include "tuple.h"
#include "run.h"
using namespace std;



int main(int argc, char *argv[]){
    
    Cudd manager;
    manager =Cudd(0,0);
   
    srand(time(0));
  
    Cudd_AutodynEnable(manager.getManager(),  CUDD_REORDER_WINDOW4);
    Cudd_ReduceHeap(manager.getManager(), CUDD_REORDER_WINDOW4, 0);
    if(strcmp(argv[1],"solver")==0){
    	inputRandom(manager,argc,argv);
      
    }
    else if(strcmp(argv[1],"benchmarks")==0){
    	inputBenchmark(manager,argc, argv);

 	}
    else if(strcmp(argv[1],"file")==0){
        inputFile(manager,argc, argv);
    }
    else
        cout<<"error: no command found\n";

    return 0;
}


