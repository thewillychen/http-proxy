SRCS=proxy.cpp LRUcache.cpp 

all: proxy

node: $(SRCS)
	g++ -std=c++0x -pthread -o proxy $(SRCS) 

clean: 
	rm -f proxy
