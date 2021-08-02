all: server client httpd
	# .PHONY: all
	#.PHONY: all httpd server client

server: server.cpp
	g++ -o server server.cpp -lpthread

client: client.cpp
	g++ -o client client.cpp

httpd:
	g++ -o httpd tinyhttpdcpp.cpp -lpthread
