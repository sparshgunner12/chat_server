chat_server
===========
Simple Peer-Peer  Chat Application
			Name-Sparsh Kumar Sinha
			Roll No-11010166

Running-
 A makefile has been provided with the code

Commands to compile-

$make

It compiles as follows-
g++ -pthread -w -o server server.cpp 
g++ -pthread -w -o client client.cpp


The code can run both locally and globally. 

Server-
The server starts at port 4201 on the host PC. It can be accessed by the PC's IP address or 127.0.0.1 if the server is on local host.
The server creates a maximum of 20 threads to deal with the clients on a separate ports.So the clients can interact simultaneously without any hiccup with the server.
To run the server you can type-

$./server

Client-
The client asks about the server's IP address. This is 127.0.0.1 if the client is local or the IP address of the computer in which client is running.
Next the user has to enter an idle port(on which no socket is running).If the socket is invalid or another program is using the socket  it shows “bind error”.If the IP:PORT has already been registered by the server then it terminates then and there.
To run client type-

$./client

Messaging Format-
The clients can enter maximum of 256 character except '#'.The message terminates on newline .If the users have to disconnect then they have to type exit() only.If the other client disconnects the the present client will have to type astray message to get out of chat mode.

The code can run globally and locally(provided the ports are unique)

NOTE:
The client requesting connection will be blocked until his/her connection gets accepted or rejected





SERVER -CLIENT INTERACTION
Part 1-Sending clients a list of available peers.
SERVER-
arsenal@arsenal-Dell-System-XPS-L502X:~/11010166$ ./server 

Socket created 
Connection accepted 
Connection accepted 
Sending 127.0.0.1:9044 

 to 
127.0.0.1:2222 
Sending 127.0.0.1:2222 

 to 
127.0.0.1:9044 
127.0.0.1:9044 has disconnected 
Sending None 

 to 
127.0.0.1:2222 
127.0.0.1:2222 has disconnected

CLIENT 1
arsenal@arsenal-Dell-System-XPS-L502X:~/11010166$ ./client 
Enter Server IP 
127.0.0.1 
Connecting to server 
Enter Your listening port 
Warning:Use an unused port(>1024) 
9044 
Creating Listen Socket for peer interaction 
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
1 
The list of available peers are 
127.0.0.1:2222
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
3

CLIENT 2
arsenal@arsenal-Dell-System-XPS-L502X:~/11010166$ ./client 
Enter Server IP 
127.0.0.1 
Connecting to server 
Enter Your listening port 
Warning:Use an unused port(>1024) 
2222 
Creating Listen Socket for peer interaction 
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
1 
The list of available peers are 
127.0.0.1:9044
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
1 
The list of available peers are 
None
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
3

Part 2-Client Client Chatting
SERVER-
arsenal@arsenal-Dell-System-XPS-L502X:~/11010166$ ./server 

Socket created 
Connection accepted 
Connection accepted 
Sending 10.11.10.60:4444 

 to 
10.11.10.60:4445 
Sending 10.11.10.60:4445 

 to 
10.11.10.60:4444 
10.11.10.60:4445 is now chatting 
10.11.10.60:4444 is now chatting 
10.11.10.60:4444 is now available 
10.11.10.60:4445 is now available 
10.11.10.60:4444 has disconnected 
10.11.10.60:4445 has disconnected

CLIENT 1-
arsenal@arsenal-Dell-System-XPS-L502X:~/11010166$ ./client 
Enter Server IP 
10.11.10.60 
Connecting to server 
Enter Your listening port 
Warning:Use an unused port(>1024) 
4444 
Creating Listen Socket for peer interaction 
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
1 
The list of available peers are 
10.11.10.60:4445 


Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
2 
Checking Listen port for incoming connections 
If you still want to connect to a peer enter 1 
Else enter any other number 
1 
Enter the peer ip: 
10.11.10.60 
Enter the peer port: 
4445 
Connecting to 10.11.10.60:4445 
Enter message no 1:Newline for terminating message,'exit()' to disconnect 
Enter message no 2:Newline for terminating message,'exit()' to disconnect 
i am fine 
Enter message no 3:Newline for terminating message,'exit()' to disconnect 
thank you very much 
Enter message no 4:Newline for terminating message,'exit()' to disconnect 
exit() 
Sending Thread Closed
Receiving Thread Closed
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
3

All the client messages are redirected to a file whose name is same as the port entered by the client .This is done because the simultaneously received messages cannot be redirected to stdout which creates an unexpected behaviour .If we create some kind of lock then the duplexity of the chat is not maintained .Also each message has a a time stamp thus showing that  it is simultaneous(Since the send and receive are run on different threads and are non-blocking)

So the CLIENT1 received file 4445 is as follows-

		Wed Feb 19 18:23:43 2014 

1 was received and displayed successfully		Wed Feb 19 18:23:44 2014 

hi zuzu 
		Wed Feb 19 18:23:57 2014 

how are you 
		Wed Feb 19 18:24:02 2014 

2 was received and displayed successfully		Wed Feb 19 18:24:10 2014 

3 was received and displayed successfully		Wed Feb 19 18:24:16 2014 






CLIENT 2-
arsenal@arsenal-Dell-System-XPS-L502X:~/11010166$ ./client 
Enter Server IP 
10.11.10.60 
Connecting to server 
Enter Your listening port 
Warning:Use an unused port(>1024) 
4445 
Creating Listen Socket for peer interaction 
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
1 
The list of available peers are 
10.11.10.60:4444 


Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
2 
Checking Listen port for incoming connections 
selectserver: new connection from 10.11.10.60 on socket 5 
Enter 1 to accept 
1 
Enter message no 1:Newline for terminating message,'exit()' to disconnect 
Enter message no 2:Newline for terminating message,'exit()' to disconnect 
hi zuzu 
Enter message no 3:Newline for terminating message,'exit()' to disconnect 
how are you 
Enter message no 4:Newline for terminating message,'exit()' to disconnect 
Receiving Thread Closed
sdsadsa 
Sending Thread Closed
If you still want to connect to a peer enter 1 
Else enter any other number 
2 
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
3

CLIENT 2 4445 file-


		Wed Feb 19 18:23:44 2014 

1 was received and displayed successfully		Wed Feb 19 18:23:44 2014 

2 was received and displayed successfully		Wed Feb 19 18:23:58 2014 

3 was received and displayed successfully		Wed Feb 19 18:24:03 2014 
 
i am fine 
		Wed Feb 19 18:24:09 2014 

thank you very much 
		Wed Feb 19 18:24:15 2014

Some Errors and Bugs-
ERRORS
1)
arsenal@arsenal-Dell-System-XPS-L502X:~/11010166$ ./client 
Enter Server IP 
10.11.10.60 
Connecting to server 
Enter Your listening port 
Warning:Use an unused port(>1024) 
2222 
Your IP:PORT already in use 
Terminating connection

If the port is already bounded by another socket.

2)
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
1 
The list of available peers are 
10.11.10.60:2222 


Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
2 
Checking Listen port for incoming connections 
If you still want to connect to a peer enter 1 
Else enter any other number 
1 
Enter the peer ip: 
10.11.10.60 
Enter the peer port: 
2222 
Connecting to 10.11.10.60:2222 
connect: Connection refused 
connection error

If the client tries to connect to another client which has just left the server.Refresh the list to see that it is unavailable.
Enter 1 for peer-list,2 to connect to a peer,3 to disconnect 
1 
The list of available peers are 
None

BUGS-
1)
If the clients disconnect without typing the exit() message then the output is unexpected.

2)
If the list is not refreshed and the client tries to connect to another client who is busy then the socket gets blocked until the busy client is done with his conversation.

3)
Sometimes some garbage value might get printed in the received file.

4)
There is no checking if the desired number of bytes have reached the client.

