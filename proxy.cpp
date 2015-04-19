#include <iostream>
#include <string>
#include "LRUcache.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "proxy.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <regex>
#include <pthread.h>

using namespace std;

Proxy::Proxy(string pport, char * cacheSizeMB){
	port = pport;
	cacheSize = atoi(cacheSizeMB);
	cache = LRUcache(cacheSize);
}

void* callProcessRequest(void* object){
	threadParams * parameters = new threadParams;
	parameters = (threadParams*)object;
	parameters->thisProxy->processRequest(parameters->requestMsg, parameters->socket);
	//delete parameters;
	return NULL;
}

int Proxy::initBrowserListener(){
	//Do something to listen to the port and receive the http requests from the browser
	char buf[MAX_MSG_LENGTH];
	struct sockaddr_in sin;
	int serv_sock, new_sock;
	socklen_t len;

	bzero((char*)&sin, sizeof(sin));
	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr= INADDR_ANY;
	sin.sin_port = htons(atoi(port.c_str()));
	/*Passive listen setup*/
	if((serv_sock = ::socket(PF_INET, SOCK_STREAM,0)) <0){
		perror("Create socket error");
		exit(1);
	}

	if((::bind(serv_sock, (struct sockaddr *)&sin, sizeof(sin))) < 0){
		perror("Bind error");
		exit(1);
	}
	::listen(serv_sock, 5);
	/*Wait for connection loop*/
	while(1){
		len = sizeof(sin);
		fprintf(stderr, "Server is listening\n");
		//new_sock = browser - proxy connection 
		if((new_sock = ::accept(serv_sock, (struct sockaddr *)&sin, &len))<0){
			perror("Connection failed");
			exit(1);
		}
		//browserfd = new_sock;

		fprintf(stderr, "new connection\n");
		if(len = ::recv(new_sock,buf,sizeof(buf),0)){ //while potentially for persistent?
			// fprintf(stderr, "receieved:\n%s\n", buf);
			// string request(buf);
			// fprintf(stderr,"Url: %s",parseURL(request).c_str());
			//new_sock = browser 
			threadParams params;
			params.thisProxy = this;
			params.socket = new_sock;
			memset(params.requestMsg, 0, MAX_MSG_LENGTH);
			memcpy(params.requestMsg, buf, MAX_MSG_LENGTH);

			pthread_t httpRequestThread;
			pthread_create(&httpRequestThread, NULL, &callProcessRequest,(void*)&params);
		}
	}

	return 0;	
}

int Proxy::processRequest(char * msg, int socket){
	string request(msg);
	string url = parseURL(request);
	char reply[];

	int sockfd;  
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int recv_len =0;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(url, "http", &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("socket");
		continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("connect");
			continue;
		}

		if(send(socketfd,msg,MAX_MSG_LENGTH, 0)< 0){
			perror("Send error:");
			return l;
		}
		recv_len = read(socketfd,reply, MAX_MSG_LENGTH*10);
		if (recv_len < 0) {
			perror("Recv error:");
			return 1;
		}
		fprintf(stderr, "Server response: %s\n", reply);

	    break; // if we get here, we must have connected successfully
	}

	if (p == NULL) {
	    // looped off the end of the list with no connection
		fprintf(stderr, "failed to connect\n");
		exit(2);
	}

	freeaddrinfo(servinfo); // all done with this structure
		//fprintf(stderr, "%s\n", url.c_str());
		//Respond to the http request 
		//check cahce
	return 1;
}

string Proxy::parseURL(string request){
	std::vector<std::string> header; 
	string copyOfRequest = strdup(request.c_str());
	string delimiter = "\r\n";
	printf("Parsing url");
	size_t pos = 0;
	std::string token;
	while ((pos = copyOfRequest.find(delimiter)) != std::string::npos) {
	    token = copyOfRequest.substr(0, pos);
	 	fprintf(stderr, "%s\n", token.c_str());
	    header.push_back(token);
	    copyOfRequest.erase(0, pos + delimiter.length());
	}

	header.push_back(copyOfRequest);

	string url = string();
	string host = "Host: ";
	for(auto i : header){
		size_t found = i.find(host);
		if(found!=std::string::npos){
			url = i.substr(host.length());
		}
	}
	return url;
}


int main(int argc, char ** argv){
	if(argc != 3){
		cerr << "Incorrect number of inputs" << endl;
		return 1;
	}
	Proxy p = Proxy(argv[1],argv[2]);
	p.initBrowserListener();
	return 0;
}




