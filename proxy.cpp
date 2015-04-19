#include <iostream>
#include <string>
#include "LRUcache.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "proxy.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

Proxy::Proxy(string pport, char * cacheSizeMB){
	port = pport;
	cacheSize = atoi(cacheSizeMB);
	cache = LRUcache(cacheSize);
}

sockaddr_in sAdr(string str){
  struct sockaddr_in addr;
  int s=str.find(":");
  addr.sin_addr.s_addr = inet_addr(str.substr(0,s).c_str());
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(str.substr(s+1).c_str()));
  return addr;
}


int Proxy::listenForBrowser(){
	//Do something to listen to the port and receive the http requests from the browser
	char buf[8190];
	struct sockaddr_in sin;
	int serv_sock, new_sock;
	int len;

	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htons(INADDR_ANY);
	sin.sin_port = htons(port);

	/*Passive listen setup*/
	if((serv_sock = socket(PF_INET, SOCK_STREAM,0)) <0){
		perror("Create socket error");
		exit(1);
	}

	if((bind(serv_sock, (struct sockaddr *)&sin, sizeof(sin))) < 0){
		perror("Bind error");
		exit(1);
	}
	listen(serv_sock, MAX_PENDING);
	printf("Server is listening");

	/*Wait for connection loop*/
	while(1){
		if((new_sock = accept(serv_sock, (struct sockaddr *)&sin, &len))<0){
			perror("Connection failed");
			exit(1);
		}
		while(len = recv(new_sock,buf,sizeof(buf),0)){
			printf("receieved \n");
		}
	}

	return 0;	
}

int Proxy::respond(char * msg){
	//Respond to the http request 
	//check cahce
	return 1;
}

char * Proxy::parseHTTP(char * msg){
	return NULL;
}

int Proxy::run(){
	listenForBrowser();
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




