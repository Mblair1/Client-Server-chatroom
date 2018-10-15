#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include <pthread.h>

#define SERVER_PORT 7900
#define MAX_LINE 256
/********************************************************
Project goal:
This is the client file that had the only function of sending three registration packets to the server,
receives an acknowledgement from the server, and then receives a chat packet and displayes the chat packet's
data.

Due date: 20 February 2018
Last updated: 20 February 2018

Author(s): Blair, Matthew Bryce
References: Help and assistance for this code was supplied by the following:
Andrus
Ballard
Jeffords

Each person listed gave hints, not code.
********************************************************/
void *receive_thread(int *sock)
{
	int new_sock = *sock;
	while(1)
	{
		if(recv(new_sock, &pakcet_chat,sizeof(pakcet_chat),0) < 0)
		{
			printf("\n chat packet failed to be received. \n ");
			eixt(1);
		}
		else 
		{
			printf("\n" + pakcet_chat.data + "\n");
		}
	}
}
int main(int argc, char* argv[])
{
        //Construction of a packet
        struct packet {
				short seqNumber;
                short type;
                char uName[MAX_LINE];
                char mName[MAX_LINE];
                char data[MAX_LINE];
				int groupNum;
        };

        //Variables are declared here that is used throughout the code
        struct packet packet_reg;
        packet_reg.type = htons(121);
		struct packet packet_conf;
		packet_conf.type = htons(221);
		struct packet packet_chat;
        packet_chat.type = htons(131);
		
		
		 //variables that are used in the client
        char clientname[MAX_LINE];
        clientname[MAX_LINE];
        struct hostent *hp;
        struct sockaddr_in sin;
        char *host;
		char buf{MAX_LINE];
        int s;
        int len;
		int capacity = 0;

        gethostname(clientname, MAX_LINE); 

      
        if(argc == 4)
        {
                host = argv[1];
		}
        else
        {
                fprintf(stderr, "usage:newclient server\n");
                exit(1);
        }

        /* translate host name into peer's IP address */
        hp = gethostbyname(host);
        if(!hp)
        {
                fprintf(stderr, "unkown host: %s\n", host);
                exit(1);
        }

        /* active open */
        if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        {
                perror("tcpclient: socket");
                exit(1);
        }

        /* build address data structure */
        bzero((char*)&sin, sizeof(sin));
        sin.sin_family = AF_INET;
        bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
        sin.sin_port = htons(SERVER_PORT);

        //check to see if the client program has connected to the server. If it
		if(connect(s,(struct sockaddr *)&sin, sizeof(sin)) < 0)
        {
                perror("tcpclient: connect");
                close(s);
                exit(1);
        }
		else
		{
			printf("\nConenction established");
		}
		
        strcpy(packet_reg.mName, clientname);
        strcpy(packet_reg.uName, argv[2]);
		strcpy(packet_reg.groupNum, argv[3]);
		//we are sending three registration packets so I use a while loop to make the code cleaner. So capacity 
		//goes and incriments until it hits three packets sent
		while(capacity < 3)
		{
		//repeated sending of the registration packet
        if(send(s,&packet_reg,sizeof(packet_reg),0) < 0)
        {
                printf("\n Registration packet failed to sen \n");
                exit(1);
        }
        else
        {
                printf("\n Registration packet has been sent \n");
        }
		capacity++; //incriment capactity so we only send so many registration packets
		}
		//once the packets have been sent, we wait and receive the acknowledgment from the server
		if(recv(s,&packet_conf,sizeof(packet_conf),0) < 0)
		{
                printf("\n Confirmation packet failed to be received \n");
                exit(1);
        }
        else
        {
                printf("\n Confirmation packet has been received\n");
        }
		
		pthread_create(&threads[0],NULL,(void*)receive_thread,&s);
		while(fgets(buf, sizeof(buf), stdin))
		{
		buf[MAX_LINE-1] = '\0';
		len = strlen(buf) + 1;
		strcpy(packet_chat.data, buf);
		strcpy(packet_uName, argv[2]);
		strcpy(packet_groupNum, argv[3]);
		if(send(s, &packet_chat, sizeof(packet_chat),)) < 0)
		{
			printf("|n Chat packet could not be sent \n" );
			exit(1);
		}
		}
		