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
using namespace std;
map<string,int>mp;
vector<int> req[10001];
bool chat[10001];
string port[10001];
pthread_mutex_t loc=PTHREAD_MUTEX_INITIALIZER,loc2=PTHREAD_MUTEX_INITIALIZER,loc3=PTHREAD_MUTEX_INITIALIZER,loc4=PTHREAD_MUTEX_INITIALIZER;
char x[300];
int si=0;
int sd,i,len,bi,don[20];
char content[30],s[256];
pthread_t t[20];
struct sockaddr_in ser,cli;
/*
This function converts character array to string
*/
string convert(char * a)
{
	string b="";
	int l=strlen(a);
	for(int i=0;i<l;i++)
	{
		b+=a[i];
	}
	return b;
}
/*
This function converts string to character array
*/
void convertb(string a,char * x)
{
	int i=0;
	for(i=0;i<a.size();i++)
	{
		x[i]=a[i];
	}
	x[i]='\n';
	i++;
	x[i]='\0';
}
/* 
This is the function which is run on a new thread on the server
If there is an incoming connection then the server starts communicating with the client indefinately.
Until the client disconnets
*/
void *newconn(void * ptr)
{
		int no=*((int *)ptr);//receive the number of the thread pointer
		pthread_mutex_unlock( &loc4 );
		struct sockaddr_in cli;
		int nsd,i=sizeof(cli),pid;
		char buf[256],s[256],x[256];
		nsd = accept(sd,((struct sockaddr *)&cli),(socklen_t*)(&i));//See if there are some connections
		//Every information about the incoming server is stored in struct cli
		if(nsd==-1)//If there is some error in accept the it returns -1
		{
			don[no]=0;
			return 0;
		}
		//If nsd!=-1 then the connection is valid
		printf("Connection accepted\n");
		inet_ntop(cli.sin_family,&(cli.sin_addr.s_addr),s, sizeof s);//Store the IP address in s character array 
		recv(nsd,buf,255,0);//Get the port number from client
		pthread_mutex_lock( &loc );//Create a lock Since mp and si are global
		pid=si;
		string is=convert(s);
		port[pid]=convert(buf);
		is+=':';
		is+=port[pid];
		//This part brings in IP and PORT and stores as IP:PORT
		if(mp.find(is)==mp.end())//This means that the IP:PORT is not present in the map
		{
			mp.insert(std::pair<string,int>(is,si));
			send(nsd,"6",1,0);//Send a message to the client to continue connection
		}
		else 
		{
			send(nsd,"5",1,0);//Send a message to the client to stop connection
			close(nsd);
			don[no]=0;
			pthread_mutex_unlock( &loc );//Release the lock
			return 0;
		}
		si++;//Increment si since now the prevoius value has already been assigned to a key
		pthread_mutex_unlock( &loc );
		while(1)
		{
			int numbytes;
			if((numbytes = recv(nsd, buf,255,0))>0)//No error in receive
			{
				buf[numbytes]='\0';
				if(buf[0]=='1')//Client is asking for peer list
				{
					map<string,int>::iterator it;
					string se="";
					pthread_mutex_lock( &loc);//Map mp is global
					for(it=mp.begin();it!=mp.end();++it)
					{
						//Dont send the client its own address and the address of the clients already chatting
						if(chat[it->second] || it->second==pid)continue;
						se+=it->first;
						se+='\n';
					}
					pthread_mutex_unlock( &loc );//Unlock lock
					if(se=="")se="None\n";
					convertb(se,x);
					cout<<"Sending "<<x<<" to\n"<<is<<endl;
					send(nsd,x,255,0);//Send the list of peers
				}
				else if(buf[0]=='2')//Client will now chat with someone else so server should wait
				{	
					chat[pid]=1;//Set the boolean chat variable to 1
					//This is done so that the future clients are not sent the 
					//address of this busy client
					cout<<is<<" is now chatting"<<endl;
					recv(nsd, buf,255,0);
					//Wait for a message to be received from the client
					//Client will send message after it is done with the chatting
					cout<<is<<" is now available"<<endl;
					chat[pid]=0;
					//The client is now available
					//So set its corresponding boolean back to 0
				}
				else if(buf[0]=='3')//Client has sent a disconnect request
				{
					cout<<is<<" has disconnected "<<endl;
					pthread_mutex_lock( &loc);//Get lock
					mp.erase(is);//Erase its IP from map
					pthread_mutex_unlock( &loc );//Unlock
					close(nsd);//Close socket
					break;
				}
			}
			else if(numbytes==0)//Means client disconnected disruptly as the connection is no longer active
			{
				cout<<is<<" has disconnected "<<endl;
				pthread_mutex_lock( &loc);
				mp.erase(is);
				pthread_mutex_unlock( &loc );
				close(nsd);
				break;

			}
		}
		don[no]=0;
}
int main()
{
	if((sd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1)//Create a TCP socket
	{
		printf("\nSocket problem");
		return 0;
	}
	printf("\nSocket created\n");
	bzero((char*)&cli,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(4201);
	ser.sin_addr.s_addr=htonl(INADDR_ANY);
	int yes=1;
	if (setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) //Try to remove the process running on this port
	{
		perror("setsockopt");
		exit(1);
	}
	bi=bind(sd,(struct sockaddr *)&ser,sizeof(ser));//Try to bind with the socket
	if(bi==-1)
	{
		printf("\n bind error, port busy, plz change port in client and server");
		printf("%d\n",errno);
		return 0;
	}
	listen(sd,20);//Listen on this socket for client connections
	int i;
	while(1)
	{
		for(i=0;i<20;i++)//This is for running a maximum of 20 threads
		{
			if(!don[i])
			{
				pthread_mutex_lock( &loc4 );//lock since variable i is shared
				don[i]=1;//To state that thread t[i] has been used
				pthread_create(&t[i],NULL,newconn,(void *)&i);//create a new thread to run newconn
				break;
			}
		}
	}
	printf("\n Bye");
	close(sd);
	return 0;
}
