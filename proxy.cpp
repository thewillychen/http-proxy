#include <iostream>
#include <string>
using namespace std;

Proxy::Proxy(string port, string cacheSize){
	Proxy::port = port;
	Proxy::cacheSize = cacheSize;
}

int Proxy::run(){
	return 1;
}

int main(int argc, char ** argv){
	if(argc != 3){
		cerr << "Incorrect number of inputs" << endl;
		return 1;
	}



