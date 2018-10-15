
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<fcntl.h>
#include<unistd.h>

#define SERVER_PORT 7900
#define MAX_LINE 256
#define MAX_PENDING 5
/********************************************************
Project goal:
This file is the server file that will accept a client. While waiting for the client to connect, the server will
create a join handler thread and a chat multicaster. The multicast acts like a radio station, transmitting a text
file to anyone who is connected. The chat multicaster will not send the text from the file until at least one client
is connected. Once the client is connected, the join handler will wait for two additional registration packets before
the join handler will input the client's information into the client_info table. Once the chat multicaster reads that
there is data in the client_info table, then it will send text from the file to everyone who is entered in on the table.
The join handler will then wait to receive and add more clients into the client_info table so that the chat multicaster
can then begin transmitting text from the file to each of the clients who are "listening" in on the chat multicaster's 
broadcast.

Due date: 19 March 2018
Last updated: 19 March 2018

Author(s): Blair, Matthew Bryce
References: Help and assistance for this code was supplied by the following:
Jeffords

Each person listed gave hints, not code.
********************************************************/
int cindex = 0; // cindex represents the number of clients connected
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;		// global mutex variable
packet buffer[MAX_LINE];
short bufferindex = 0;
//packet structure for all the packets we are sending and receiving
struct packet
        {
				short seqNumber;
                short type;
                char uName[MAX_LINE];
                char mName[MAX_LINE];
                char data[MAX_LINE];
				short groupNum;
        };
//table structure to store all the information of the clients who are connecting to the server
struct registrationTable
                {
						short seqNumber;
                        int port;
                        int sockid;
                        char mName[MAX_LINE];
                        char uName[MAX_LINE];
						int groupNum;
                };
struct registrationTable client_info[10];

//join handler thread that receives registration packets and inputs the client's data into the table
void *join_handler(struct registrationTable *clientData)
				{
					//field of variables that are used in the join handler thread
					struct sockaddr_in sin;
					struct sockaddr_in clientAddr;
					bzero((char*)&sin, sizeof(sin));
					sin.sin_family = AF_INET;    
					sin.sin_addr.s_addr = INADDR_ANY;
					sin.sin_port = htons(SERVER_PORT);
					int newsock;
					struct packet packet_reg;
					newsock = clientData->sockid;
					int arrayIndex = 0;
					
					struct packet packet_conf;
					packet_conf.type = htons(221);
					struct packet packet_chat;
					packet_chat.type = htons(131);
					//receiving the second registration packet from the client
					if(recv(newsock,&packet_reg,sizeof(packet_reg),0)<0)
					{
						printf("\n Could not receive\n");
						exit(1);
					}
					else
					{
					printf("\n Client has been registered\n");
					//receiving the third registration packet from the client
					}		
					if(recv(newsock,&packet_reg,sizeof(packet_reg),0)<0)
					{
						printf("\n Could not receive\n");
						exit(1);
					}
					else
					{
					printf("\n Client has been registered\n");
					}
					//when the two registration packets are received from the client, then the join handler will send
					//an acknowledgement packet
					if(send(newsock,&packet_conf,sizeof(packet_conf),0 ) < 0)
					{
                        printf("\n confirmation packet failed to be sent \n");
                        exit(1);
					}
					else
					{
					printf("\n Confirmation packet has been sent\n");
					}
					//lock the table so that the chat multicaster cannot read the table while the join handler
					//is updating the table
					pthread_mutex_lock(&my_mutex);
					//add the client's info into the table
					client_info[cindex].port = clientAddr.sin_port;                
					client_info[cindex].sockid = newsock;                            
					strcpy(client_info[cindex].uName, packet_reg.uName);           
					strcpy(client_info[cindex].mName, packet_reg.mName);  
					client_info[cindex].groupNum = packet_reg.groupNum;
					cindex++;	//incriment cindex so the multicaster knows how many clients are connected
					//unlock the table so that the multicaster can now get in and read the table
					pthread_mutex_unlock(&my_mutex);
					
					while(1)
					{
						if(recv(newsock, &packet_chat,sizeof(packet_chat),0) < 0) 
						{
							printf("chat packet could not be received");
						}
						else
						{
							buffer[arrayIndex] = packet_chat;
							arrayIndex++;
						}
					}
					
					
					
				}
				
				
//chat multicaster that will send data constantly while there is atleast one client connected.
void *chat_multicaster()
				{
					//different variables that are used in the chat multicaster
				struct packet temp;
					int groupNum;
					int i;
					int s;
			
					int nread;
					struct packet packet_chat;
					packet_chat.type = htons(131);
					
					
					
					
					//infinity loop so that the text file will not stop sending
					while(1)
					
					{
					//this while loop will only send if there is atleast one client connect, and will constantly
					//check to see when a client had finally connected
					while(cindex > 0)
					{
						temp = buffer[bufferindex];
						bufferindex++;
						groupNum = temp.groupNum;
					//read text from the input text file
					
					
					pthread_mutex_lock(&my_mutex);
					//a for loop that will go through the client info table and send the chat data to everone who is in the table
					for ( i = 0; i < cindex; )
					{
						if(client_info[i].groupNum = groupNum)
						{
						s = client_info[i].sockid;
						if(send(s, &packet_chat,sizeof(packet_chat),0)<0)
                        {
                                printf("Chat packet was not received \n");
                                exit(1);
                        }
						else
						{
							
						
							printf("\n\n%s ", packet_chat.data);
						}
						}
						
						//incriment i so that the chat multicaster will then send to multiple clients
						i++;
						
					}
					//unlock the table so join handler can access
					pthread_mutex_unlock(&my_mutex);
					
					}
					}
				}
				
int main(int argc, char* argv[])
{       


        
        //declare variables and initialize the types of the packets used
        struct packet packet_reg;
        packet_reg.type = htons(121);
		
		
		void *exit_value;
        struct sockaddr_in sin;
        struct sockaddr_in clientAddr;
		pthread_t threads[2]; 										//we will need two threads on the server
        int s, new_s;
        int len;
        
		//create the multicaster
		pthread_create(&threads[1],NULL,chat_multicaster,NULL);
		
		  /* setup passive open */
        if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        {
                perror("tcpserver: socket");
                exit(1);             
        }


        /* build address data structure */
        bzero((char*)&sin, sizeof(sin));
        sin.sin_family = AF_INET;    
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(SERVER_PORT);


        if(bind(s,(struct sockaddr *)&sin, sizeof(sin)) < 0)
        {
                perror("tcpclient: bind");
                exit(1);
        }
        listen(s, MAX_PENDING);
		
				//checks to see if the client is accepted
               while(1){
			   if((new_s = accept(s, (struct sockaddr *)&clientAddr, &len)) < 0)
                {
                        perror("tcpserver: accept");
                        exit(1);
                }
				else
				{
					printf("\n Client has been connected\n");
					
				}
				//accept the first registration packet so we know to make the join handler
				if(recv(new_s,&packet_reg,sizeof(packet_reg),0)<0)
				{
					printf("Registration packet failed to be received");
				}
				else
				{
					printf("\n Client has been registered\n");
					client_info[0].sockid = new_s;
				}
				
				
				
				//creating of the join handler thread in the server
				pthread_create(&threads[0],NULL,(void *)join_handler,&client_info);
			
				
	
				
			   }
				
				
				
		
}
				
			
				


				
				
				
				
				
				
				
				
				
				
				
				
				
				

