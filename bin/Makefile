$(CC)= g++
FLAGS = -I /path/to/cudd-3.0.0/cudd/cudd/ 
-I /path/to/cudd-3.0.0/cudd/util 
-I /path/to/cudd-3.0.0/cudd/ 
-I /path/to/cudd-3.0.0/cudd/mtr/ 
-I /path/to/cudd-3.0.0/cudd/st/ 
-I /path/to/cudd-3.0.0/cudd/epd/ 
-I /path/to/cudd-3.0.0/cudd/cplusplus/
LFLAGS = -L /path/to/cudd-3.0.0/cudd/cudd/.libs/ -lcudd -lm

default: prom

prom: 
	g++ symbolic.cpp run.cpp generator.cpp solvers.cpp benchmark.cpp paritygames.cpp tuple.cpp -o spgsolver $(FLAGS) $(LFLAGS)
