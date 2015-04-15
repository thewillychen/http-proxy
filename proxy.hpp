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
		Proxy(string port, string cacheSize);
		int run();
		int listen();
		int respond(char * msg);
		string parseHTTP(char * msg);
}