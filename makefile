all: server client
	# .PHONY: all

server: server.cpp
	g++ -o server server.cpp -lpthread

client: client.cpp
	g++ -o client client.cpp