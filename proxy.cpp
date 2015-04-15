#include <iostream>
#include <string>
#include "LRUcache.hpp"
using namespace std;

Proxy::Proxy(string port, string cacheSize){
	port = port;
	cacheSize = cacheSize;
	cache = LRUcache(cacheSize);
}

int Proxy::run(){
	return 1;
}

int main(int argc, char ** argv){
	if(argc != 3){
		cerr << "Incorrect number of inputs" << endl;
		return 1;
	}
	Proxy p = Proxy(argv[1],argv[2]);
	p.run();
	return 0;
}




