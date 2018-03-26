$(CC)= g++
FLAGS = -I /Users/antoniodistasio/Dropbox/cudd/cudd/ -I /Users/antoniodistasio/Dropbox/cudd/util -I /Users/antoniodistasio/Dropbox/cudd/ -I /Users/antoniodistasio/Dropbox/cudd/mtr/ -I /Users/antoniodistasio/Dropbox/cudd/st/ -I /Users/antoniodistasio/Dropbox/cudd/epd/ -I /Users/antoniodistasio/Dropbox/cudd/cplusplus/
LFLAGS = -L /Users/antoniodistasio/Dropbox/cudd/cudd/.libs/ -lcudd -lm


default: prom

prom: 
	g++ symbolic.cpp run.cpp generator.cpp solvers.cpp benchmark.cpp paritygames.cpp tuple.cpp -o spgsolver $(FLAGS) $(LFLAGS)