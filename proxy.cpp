#include <iostream>
#include <string>
#include "LRUcache.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "proxy.hpp"
using namespace std;

Proxy::Proxy(string pport, string cacheSizeMB){
	port = pport;
	cacheSize = std::stoi(cacheSizeMB,10);
	cache = LRUcache(cacheSize);
}

int Proxy::listen(){
	//Do something to listen to the port and receive the http requests from the browser
	char buf[8190];
	struct sockaddr_in my_addr=sAdr(port);
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Create socket error:");
		return -1;
	}

	if (::bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) { 
		perror("bind failed"); 
		return -1;
	}
	while(1){
		int recvlen = recvfrom(sock, buf, 8190, 0, 0, 0);

		if(recvlen>0){
			std::thread (&Proxy::respond,buf).detach();
			//newThread.detach();
		}

	}
	return 0;	
}

int Proxy::respond(char * msg){
	//Respond to the http request 
	//check cahce
}

string Proxy::parseHTTP(char * msg){

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

sockaddr_in sAdr(string str){
  struct sockaddr_in addr;
  int s=str.find(":");
  addr.sin_addr.s_addr = inet_addr(str.substr(0,s).c_str());
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(str.substr(s+1).c_str()));
  return addr;
}




