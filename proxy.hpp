#include <thread>
#include "LRUcache.hpp"
using namespace std;
class Proxy{
	string port;
	int cacheSize;
	LRUcache cache; 

	public:
		Proxy(string port, string cacheSize);
		int run();

}