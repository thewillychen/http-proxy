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
#include <unistd.h>
#include <mutex>

using namespace std;
mutex Proxy::cacheLock;
Proxy::Proxy(string pport, char * cacheSizeMB){
	port = pport;
	cacheSize = atoi(cacheSizeMB);
	cache = LRUcache(cacheSize);
}

void* callProcessRequest(void* object){
	threadParams * parameters = new threadParams;
	parameters = (threadParams*)object;
	parameters->thisProxy->processRequest(parameters->requestMsg, parameters->browserSocket);
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
		if(len = ::recv(new_sock,buf,MAX_MSG_LENGTH,0)){ //while potentially for persistent?
		//	fprintf(stderr, "receieved:\n%s\n", buf);
			string request(buf);
		//	fprintf(stderr,"Url: %s",parseURL(request).c_str());
		//	new_sock = browser 
			fprintf(stderr, "spawing thread \n");
			threadParams params;
			params.thisProxy = this;
			params.browserSocket = new_sock;
			memset(params.requestMsg, 0, MAX_MSG_LENGTH);
			memcpy(params.requestMsg, buf, MAX_MSG_LENGTH);

			pthread_t httpRequestThread;
			pthread_create(&httpRequestThread, NULL, &callProcessRequest,(void*)&params);
			pthread_join(httpRequestThread, NULL);
		//	close(new_sock);
			//processRequest(buf, new_sock);
		}
	}

	return 0;	
}

int Proxy::processRequest(char * msg, int browserSocket){
	
	string request(msg);
	string url= parseURL(request);
	if(url.compare("ignore_")==0){
		fprintf(stderr, "%s\n", msg );
		return 1;
	}
	fprintf(stderr, "thread url %s\n pid %d ", url.c_str(), getpid());
	// cacheLock.lock();
	// char * creply = cache.get(url);
	// if(creply != NULL){
	// 	if(send(browserSocket, creply, strlen(creply), 0) <0){
	// 		perror("Send error");
	// 		return 1;
	// 	}
	// 	return 1;
	// }
	// cacheLock.unlock();
	char reply[MAX_MSG_LENGTH*100];

	int sockfd;  
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int recv_len =0;
	int rcvd =-1;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	string getReq = parseGetUrl(request);
	cacheLock.lock();
	char* cachedResponse = cache.get(getReq);
	cacheLock.unlock();
	if(cachedResponse!= NULL){
		int len = cache.getLength(getReq);
		int sl = send(browserSocket, cachedResponse, len, 0);
		if(sl<0){
			perror("Send error");
			return 1;
		}
		close(browserSocket);

		return 1;
	}
	if ((rv = getaddrinfo(url.c_str(), "http", &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

// loop through all the results and connect to the first we can
	
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = ::socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("socket");
		continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("connect");
			continue;
		}

		if(send(sockfd,msg,MAX_MSG_LENGTH, 0)< 0){
			perror("Send error:");
			return 1;
		}
		fprintf(stderr, "msg %s\n", msg);
		int length =0;
		while(rcvd < length){
			char rcvreply[MAX_MSG_LENGTH * 100];
			recv_len = read(sockfd,rcvreply, (MAX_MSG_LENGTH*100));
		//	fprintf(stderr, "Server response: %s\n", rcvreply);
			if (recv_len < 0) {
				perror("Recv error:");
				return 1;
			}
			  if(rcvd == -1){
				rcvd = 0;
				string response(rcvreply);
				length = getLength(response);
					

			}
			if(recv_len == 0){
				break;
			}
			copy(rcvreply, rcvreply+recv_len,reply+rcvd);
			fprintf(stderr, "Server response: %s \n reply: %s\n", rcvreply, reply);
			rcvd = rcvd + recv_len;
		   fprintf(stderr, "reclen %d, reply len %d  len %d recvd %d\n", recv_len, (int)strlen(reply), length, rcvd);

		}
		cacheLock.lock();
		cache.set(getReq, reply);
		cache.setLength(getReq, rcvd);
		cacheLock.unlock();
		fprintf(stderr, "Server response: %s\n", reply);
		int sl = send(browserSocket, reply, rcvd, 0);
		if(sl<0){
			perror("Send error");
			return 1;
		}
	//	fprintf(stderr, "Sent reply to browser bytes %d \n", sl);
	    break; // if we get here, we must have connected successfully
	}

	if (p == NULL) {
	    // looped off the end of the list with no connection
		fprintf(stderr, "failed to connect\n");
		
		exit(2);
	}
	close(sockfd);
	close(browserSocket);
	freeaddrinfo(servinfo); // all done with this structure
		fprintf(stderr, "exited %d\n", getpid());
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
	 	//fprintf(stderr, "%s\n", token.c_str());
	    header.push_back(token);
	    copyOfRequest.erase(0, pos + delimiter.length());

	}

	header.push_back(copyOfRequest);
	string url = string();
	string host = "Host: ";
	string ignore = "ignore_";
	for(auto i : header){
		size_t post = i.find("POST");
		size_t put = i.find("PUT");
		if(post != std::string::npos || put != std::string::npos ){
			url = ignore;
			break;
		}
		size_t found = i.find(host);
		if(found!=std::string::npos){
			url = i.substr(host.length());
			break;	
		}
	}
	return url;
}

string parseGetURL(string request){
	std::vector<std::string> header; 
	string copyOfRequest = strdup(request.c_str());
	string delimiter = "\r\n";
	printf("Parsing url");
	size_t pos = 0;
	std::string token;
	while ((pos = copyOfRequest.find(delimiter)) != std::string::npos) {
	    token = copyOfRequest.substr(0, pos);
	 	//fprintf(stderr, "%s\n", token.c_str());
	    header.push_back(token);
	    copyOfRequest.erase(0, pos + delimiter.length());

	}

	header.push_back(copyOfRequest);
	string url = string();
	string get= "GET ";
	string ignore = "ignore_";
	for(auto i : header){
		size_t post = i.find("POST");
		size_t put = i.find("PUT");
		if(post != std::string::npos || put != std::string::npos ){
			url = ignore;
			break;
		}
		size_t found = i.find(get);
		if(found!=std::string::npos){
			url = i;
			break;	
		}
	}
	return url;
}

int Proxy::getLength(string request){
	std::vector<std::string> header; 
	string copyOfRequest = strdup(request.c_str());
	string delimiter = "\r\n";
	printf("Parsing length");
	size_t pos = 0;
	std::string token;
	while ((pos = copyOfRequest.find(delimiter)) != std::string::npos) {
	    token = copyOfRequest.substr(0, pos);
	 	//fprintf(stderr, "%s\n", token.c_str());
	    header.push_back(token);
	    copyOfRequest.erase(0, pos + delimiter.length());

	}

	header.push_back(copyOfRequest);
	int length = 0;
	string len = "Content-Length: ";
	for(auto i : header){
		size_t found = i.find(len);
		if(found!=std::string::npos){
			length = stoi(i.substr(len.length()));
			break;	
		}
	}
	return length;
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




