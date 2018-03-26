#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <util.h>
#include <cudd.h>
#include <cuddObj.hh>
#include <cmath>
#include <string>
#include <sstream>
#include <unordered_map>

#include "tuple.h"


using namespace std;


vector<double> inc_tuple(vector<double> d, int l, vector<int> n_nodes){

	int flag=0;
	int n=d.size();
	int init;

	if(l == 0 || l==1 )
		init=1;
	else if(l%2 == 1)
		init=l;

	if(d[0]==INFINITY)
        return d;

    if(l==0 || l==1 || l%2 ==1){ // x < y 
	
    	for (int i=init;i<n;i+=2){
    		if(d[i]!=n_nodes[i]){
    			d[i]++;
    			flag=1;
    			break;    			
    		}
    		else{
    			for(int j=i+2;j<n;j+=2){
	    			if(d[j]!=n_nodes[j]){
	    				for(int y=j-2;y>=init;y-=2)
	    					d[y]=0;

	    				d[j]++;
	    				flag=1;
	    				break;
	    			}
	    		}
    		}
    		if(flag)
    			break;
    	}
    	if(!flag){
    		vector<double> inf(1,INFINITY);
    		return inf;
    	}

	}
    else{ // inc_l(x)=<x>_l

    	for(int y=1;y<l;y+=2)
    		d[y]=0;

    }
  
	return d;

}

vector<double> dec_tuple(vector<double> d, int l, vector<int> n_nodes){

	int n=d.size();
	int flag=0;
	int init;

	if(l==0)
		init=1;
	else
		init=l;

	if(d[0]==INFINITY){
		vector<double> tmp(n_nodes.size());

		for(int i=init; i<n_nodes.size(); i+=2){
			tmp[i]=n_nodes[i];
		}

		return tmp;

	}
	for(int i=init;i<n;i+=2){
		if(d[i]!=0){
			d[i]--;
			break;
		}
		else{
    		for(int j=i+2;j<n;j+=2){
	    		if(d[j]!=0){
	    			for(int y=j-2;y>=init;y-=2)
	    				d[y]=n_nodes[y];

	    				d[j]--;
	    				flag=1;
	    				break;
	    			}
	    		}
    		}
    		break;
	}


	return d;
}

int compareTuple(vector<double> t1, vector<double> t2, int l){

	int i,j, min;
	int comp, flag=0;

	int s1=t1.size();
	int s2=t2.size();
	for(i=0;i<=l;i++){
		if(i<s1 && i<s2){
			if(t1[i] < t2[i]){
				comp=-1;
				flag=1;
				break;
			}
			else if(t1[i]>t2[i]){
				comp=1;
				flag=1;
				break;
			}
		}
		else
			break;
	}
	if(flag)
		return comp;
	else{ // l>s1 or l>s2 and t1=t2  
		if(s1>s2)
			return 1;
		else if(s1<s2)
			return -1;
		else 
			return 0;
	}
	return 0;
}

vector<double> inc(vector<double> d, int l, int j, vector<int> n_nodes){

	int i,flag=0;
	vector<double> d_prime(l+1); 
    int dim;

    if(d[0] == n_nodes.size() || d[0]==INFINITY)
        return d;

    int size= d.size();
  
	if(d[0]==-INFINITY){
		for(i=0;i<=l;i++){
			d_prime[i]=0;
		}
        return d_prime;
	}
	else{
        if(size<l+1){
            for(i=size; i<=l;i++){
                d_prime[i]=0;
            }
            flag=1;
            i=size;
        }
        else{
    		for(i=l;i>=1;i--){

    			if(d[i]!=(n_nodes[j]+1)){ //controllare n_j + 1 -> n_j
    				d_prime[i]=d[i]+1;
    				flag=1;
    				break;
    			}
    			else
    				d_prime[i]=d[i];
    		}
        }
	}

	if(!flag)
	{
		vector<double> inf(1,INFINITY);
		return inf;
	}
	else{
		for(j=i-1;j>=0;j--)
			d_prime[j]=d[j];     
	}
	
	return d_prime;
}
double associateKey(vector <double> t, int cardV){
	

    if(t[0]==-INFINITY)
        return -INFINITY;
    if(t[0]==+INFINITY)
        return +INFINITY;
    if(t[0]!=0)
        return 0; // undefined
   

    int i,j,key,dim = t.size();

    if(dim-2!=0)
        key=pow(cardV+1,dim-2);
    else
        key=0;

    i=0;
	for (j=(t.size())-1;j>=1;j--){

        key= key + (t[j] * pow((cardV+1),i));
        i++;
    }
    
    key++;

    return key;
}
