all:
	g++ -pthread -w -o server server.cpp
	g++ -pthread -w -o client client.cpp