#ifndef PROXY_HPP
#define PROXY_HPP
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "LRUcache.hpp"

using namespace std;
class Proxy{
	string port;
	int cacheSize;
	LRUcache cache; 

	public:
		Proxy(string pport, string cacheSizeMB);
		int run();
		int listen();
		int respond(char * msg);
		string parseHTTP(char * msg);
};
#endif /* PROXY_HPP */