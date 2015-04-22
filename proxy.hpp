#ifndef PROXY_HPP
#define PROXY_HPP
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "LRUcache.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <regex>
#include <pthread.h>
#include <sys/socket.h>
#include <mutex>
#define MAX_MSG_LENGTH 8190

using namespace std;



class Proxy{
	string port;
	//int browserfd;
	int cacheSize;
	LRUcache cache; 
	static mutex cacheLock;
	public:
		Proxy(string pport, char * cacheSizeMB);
		int initBrowserListener();
		int processRequest(char * msg, int socket);
		string parseURL(string request);
		int getLength(string request);
};

struct threadParams{
	Proxy* thisProxy;
	char requestMsg[MAX_MSG_LENGTH];
	int browserSocket;
};
typedef struct threadParams threadParams;

#endif /* PROXY_HPP */