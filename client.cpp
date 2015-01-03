#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<signal.h>
#include<map>
#include<iostream>
#include<vector>
#include<pthread.h>
#include<fcntl.h>
#include<time.h>
using namespace std;
pthread_mutex_t loc=PTHREAD_MUTEX_INITIALIZER;
int conn,server;
char myip[256],PORT[256];
int newserconn(int port,char x[20])//Connect to PORT port and IP address in x and return the socket file descriptor
{
	int sd,con;
	struct sockaddr_in cli,cli2;
	if((sd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1)//Create a new TCP socket
	{
		printf("Socket problem\n");
		return -1;
	}
	bzero((char*)&cli,sizeof(cli));
	cli.sin_family = AF_INET;
	cli.sin_port=htons(port);//Set the server port
	cli.sin_addr.s_addr=inet_addr(x);//Set the server IP
	con=connect(sd,(struct sockaddr*)&cli,sizeof(cli));//Connect to the IP and port
	if(con==-1)
	{
		perror("connect");
		printf("connection error\n");
		return -1;
	}
	return sd;//Return the socket file descriptor
}
/*
Type cast the general struct sockaddr to sockaddr_in
if it is IP4 or to sockaddr_in if it is IP6
*/
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void* sen(void *ptr)//Function to be put in sending thread
{
	int sd=*((int *)ptr);
	char msg[256];
	int co=0;
	getchar();//Always used to remove the stray '\n' in fgets
	while(conn)
	{
		co++;
		printf("Enter message no %d:Newline for terminating message,'exit()' to disconnect\n",co);
		fgets(msg,sizeof(msg),stdin);//Get the message
		char totmsg[266];
		snprintf(totmsg, sizeof(totmsg), "%s#%d",msg, co);
		/* Encode the message entered a  message#message-number
		This is useful for recognizing acknowledgement*/
		if(strcmp(msg,"exit()\n")==0)//If msg is exit the set conn to 0 indicating that its time to close
		{
			conn=0;
			send(sd,totmsg,255,0);//Send exit() to the other client to make it exit as well
		}
		
		if(conn)
			send(sd,totmsg,255,0);//Send the message to the other client if the connection is still valid
	}
	cout<<"Sending Thread Closed"<<endl;
}
void* rev(void *ptr)//Receiving thread
{
	int sd=*((int *)ptr);
	char msg[256];
	char curtime[100];// This is to store when the message was received
	time_t ticks;
	FILE *fp;
	fp=fopen(PORT,"w");
	fclose(fp);

	while(conn)//Go on looping till the connection is active
	{
		
		int l;
		l=recv(sd,msg,255,0);
		char actmsg[256];
		char count[256];
		memset(count,0,sizeof(count));
		if(l<=0)//There is some erroe
		{
			sleep(1);
			continue;
		}
		bool c=0;
		int k=0;	
		/* 
		This is to parse
		the message#message number as 
		actualmessage-to display 
		messagenumber-to send acknowledgement
		*/
		for(int j=0;msg[j];j++)
		{
			if(msg[j]=='#')
			{
				actmsg[j]='\0';
				c=1;
				continue;
			}
			if(!c)
				actmsg[j]=msg[j];
			else count[k]=msg[j],k++;
		}
		strcat(count," was received and displayed successfully");//count stores acknowledgement
		if(k!=0)		//checking ACK
			send(sd,count,255,0);
		if(strcmp(actmsg,"exit()\n")==0)
		{
			conn=0;
			continue;
		}
		msg[l]='\0';
		fp=fopen(PORT,"a");//Store the message in file PORT(as entered by the user)
		ticks=time(NULL);
		snprintf(curtime, sizeof(curtime), "\t\t%.24s\r\n", ctime(&ticks));//Along with the current time
		fputs(actmsg,fp);//put actual message 
		fputs(curtime,fp);//put actual time
		fputs("\n",fp);
		fclose(fp);
	}
	cout<<"Receiving Thread Closed"<<endl;
}
//This function sets the socket descriptor fd as non blocking or blocking
//Depending upon the input
bool nonblck(int fd, bool blocking)
{
   if (fd < 0) return false;
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags < 0) return false;
   flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
   return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
}
fd_set master;
fd_set read_fds;
int fdmax;
// master file descriptor list
// temp file descriptor list for select()
// maximum file descriptor number
int listener;
// listening socket descriptor
int newfd;
// newly accept()ed socket descriptor
struct sockaddr_storage remoteaddr; // client address
socklen_t addrlen;
char buf[256];
int nbytes;
// buffer for client data
char remoteIP[INET6_ADDRSTRLEN];
int yes=1;
int i, j, rv;
// for setsockopt() SO_REUSEADDR, below
struct addrinfo hints, *ai, *p;
int newlisconn()
{
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	// clear the master and temp sets
	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) 
	{
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}
	for(p = ai; p != NULL; p = p->ai_next) 
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) 
		{
			continue;
		}
		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) 
		{
			close(listener);
			continue;
		}
		break;
	}
	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}
	freeaddrinfo(ai); // all done with this
	// listen
	if (listen(listener, 1) == -1) 
	{
		perror("listen");
		exit(3);
	}
	// add the listener to the master set
	FD_SET(listener, &master);
	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one
}
int listen()
{
	read_fds = master; // copy it
	struct timeval tv;
	tv.tv_sec=1;
	tv.tv_usec=0;
	if (select(fdmax+1, &read_fds, NULL, NULL, &tv) == -1) 
	{
		perror("select");
		exit(4);
	}
	// run through the existing connections looking for data to read_fds
	for(i = 0; i <= fdmax; i++) 
	{
		if (FD_ISSET(i, &read_fds)) 
		{ // we got one!!
			if (i == listener) 
			{
				// handle new connections
				addrlen = sizeof remoteaddr;
				newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);
				if (newfd == -1) 
				{
					perror("accept");
				} 
				else 
				{
					printf("selectserver: new connection from %s on ""socket %d\n",inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*)&remoteaddr),remoteIP, INET6_ADDRSTRLEN),newfd);
					printf("Enter 1 to accept\n");
					int c;
					scanf("%d",&c);
					if(c==1)
					{
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) 
						{
							// keep track of the max
							fdmax = newfd;
						}
						send(server,"2",255,0);//Send server that it is entering chat mode
						void* retval;
						send(newfd,"1",2,0);//Send the client that they can start chatting
						nonblck(newfd,0);//Set the socket to non blocking
						conn=1;
						pthread_t t1,t2;
						pthread_create(&t1,NULL,sen,(void *)&newfd);//Create a send thread
						pthread_create(&t2,NULL,rev,(void *)&newfd);//Create a receive thread
						pthread_join(t1,&retval);//Synchronize Both the threads
						pthread_join(t2,&retval);
						close(newfd);//Close the socket descriptor
						FD_CLR(newfd, &master);//Clear it from master key
						send(server,"4",255,0);//Send server that they can start communucating again
					}
					else
					{
						send(newfd,"0",2,0);
						close(newfd);
						break;
					}
				}
			}	 
		}
	}
}
int main()
{
	int sd,sd2,con,port,i,numbytes,ch;
	char content[30],buf[256],sip[256];
	printf("Enter Server IP\n");//Enter  the IP of the server
	scanf("%s",sip);
	printf("Connecting to server\n");
	sd=newserconn(4201,sip);//Connect on port 4201
	if(sd==-1)return 0;
	server=sd;
	printf("Enter Your listening port\n");
	printf("Warning:Use an unused port(>1024)\n");
	scanf("%s",PORT);//Scan the port
	send(sd,PORT,255,0);//Send the port number to the server
	recv(sd,content,30,0);//Receive the response of the server
	if(content[0]=='5')//The server already has the above IP
	{
		printf("Your IP:PORT already in use\n");
		printf("Terminating connection\n");
		return 0;
	}
	printf("Creating Listen Socket for peer interaction\n");
	sd2=newlisconn();//Create a listen socket
	while(1)
	{
		printf("Enter 1 for peer-list,2 to connect to a peer,3 to disconnect\n");
		scanf("%d",&ch);//Scan Clients's choice
		if(ch==1)
		{
			send(sd,"1",255,0);//Request server for available peers
			printf("The list of available peers are\n");
			if((numbytes = recv(sd, buf, 255,0))!=-1)//Receive the message from server
			{
				buf[numbytes]='\0';
				printf("%s\n",buf);//Printf the received message
			}
		}
		else if(ch==2)//The client wants to connect
		{
			int cho,por;
			printf("Checking Listen port for incoming connections\n");
			listen();//See if it has some requests
			printf("If you still want to connect to a peer enter 1\nElse enter any other number\n");
			scanf("%d",&cho);//Scan choice
			if(cho!=1)continue;
			printf("Enter the peer ip:\n");
			scanf("%s",buf);//Scan the peer IP
			printf("Enter the peer port:\n");
			scanf("%d",&por);//Scan the peer Port 
			int sd3;
			char msg[256];
			printf("Connecting to %s:%d\n",buf,por);
			sd3=newserconn(por,buf);//Set up a connection
			//Note:Same function can set connection to any server
			if(sd3==-1)
			{
				continue;
			}
			else
			{
				numbytes=recv(sd3,msg,255,0);//Receive the message from peer
				if(msg[0]=='0')//This means that peer does not want to talk to you
				{
					printf("Your connection was rejected\n");
					close(sd3);//close socket

				}
				else
				{
					send(sd,"2",255,0);//Else start messaging
					void * retval;
					nonblck(sd3,0);
					conn=1;
					pthread_t t1,t2;//Same as  earlier
					pthread_create(&t1,NULL,sen,(void *)&sd3);
					pthread_create(&t2,NULL,rev,(void *)&sd3);
					pthread_join(t1,&retval);
					pthread_join(t2,&retval);
					close(sd3);
					send(sd,"4",255,0);
				}
			}
		}
		else if(ch==3)//Client wants to disconnect
		{
			send(sd,"3",255,0);//Send server that it wants to disconnect
			close(sd);//Close socket
			break;//break from infinite loop
		}
		else 
		{
			printf("Invalid Option\n");
		}
	}
	return 0;
}