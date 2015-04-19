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

using namespace std;
class Proxy{
	string port;
	int cacheSize;
	LRUcache cache; 

	public:
		Proxy(string pport, char * cacheSizeMB);
		int run();
		int listenForBrowser();
		int respond(char * msg);
		char * parseHTTP(char * msg);
		string parseURL(string request);
};
#endif /* PROXY_HPP */