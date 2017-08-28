/*  UDP iterative architecture implemented in C language
	@author-Thaila Annamalai
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>


#define BUFF_SIZE 4096
#define EOM "------------------------\n"



int main(int argc,char* argv[]){
	//checks for proper command line arguments
	if(argc< 6){
		printf("USAGE: ./client server_address port_number exec_count time_delay command\n");
		exit(1);
	}

			
	//declaration of all socket variables				
	int sockfd,ginfo,sen,num,len,tim,exe,rxd,j=0,k,cmd_len=0,rem;
	char *buff;
	char *exec_count;
	char *time_delay;
	char *command,lengStr[3],rx_lengStr[5];
	struct addrinfo hints,*serverinfo,*i;
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;
	
	
	//gets the information about the server port and server IP  
	if((ginfo=getaddrinfo(argv[1],argv[2],&hints,&serverinfo))!=0){
		printf("Error in retriving information from IP address\n");
		exit(1);
	}
	
	//opening socket
	for(i=serverinfo;i!=NULL;i=i->ai_next){
		if((sockfd=socket(i->ai_family,i->ai_socktype,i->ai_protocol))==-1){
			continue;
		}
		break;
	}	
	
	if(i==NULL){
		printf("Socket failed to open\n");
		return EXIT_FAILURE;
	}
	
	buff = (char*)calloc(sizeof(char),BUFF_SIZE);
	memset(buff,'\0',BUFF_SIZE);		
	exec_count= (char*)calloc(sizeof(char),3);
	time_delay= (char*)calloc(sizeof(char),3);
	for (k = 5; k < argc; k++)
		cmd_len += strlen(argv[k])+1;
	
	command= (char*)calloc(sizeof(char),cmd_len);
	memset(command,'\0',cmd_len);
	//copies the command line arguments
	strcpy(exec_count,argv[3]);
	strcpy(time_delay,argv[4]);		
	strcpy(command,argv[5]);
	for (k = 6; k <argc;k++){
		strcat(command," ");
		strcat(command,argv[k]);
	}
	tim=atoi(time_delay);
	exe=atoi(exec_count);
	printf("Command is: %s\n",command);
	//forms the message to be sent
	
	sprintf(exec_count,"%.3d",exe);
	sprintf(time_delay,"%.3d",tim);
	len=strlen(exec_count)+strlen(time_delay)+strlen(command)+3;
	sprintf(lengStr,"%.3d",len);
	char* mes =(char*)calloc(sizeof(char),len);
	strcpy(mes,lengStr);
	strcat(mes,exec_count);
	strcat(mes,time_delay);
	strcat(mes,command);
	
	
	//sends the message to the server
	if((sen=sendto(sockfd,mes,len,0,i->ai_addr,i->ai_addrlen))==-1){
		printf("message not sent\n");
		close(sockfd);
		exit(1);
	}
	
	printf("Message sent to server\n\n");
	buff = (char*)calloc(sizeof(char),BUFF_SIZE);
	memset(buff,'\0',BUFF_SIZE);
	while(exe>0){
		j =0;
		//receives the response from the server
		if((num=recvfrom(sockfd,buff,BUFF_SIZE,0,i->ai_addr,&i->ai_addrlen))==-1){
			printf("response not received from server\n");
			continue;
		}
		printf("Rxd: %s\n",buff);
		//finds the length of the data 
		strncpy(rx_lengStr,buff,5);
		len=atoi(rx_lengStr);
		
		//forms packet to accomodate the data
		int pkt=len/BUFF_SIZE;
		
		if((rem = len%BUFF_SIZE)!=0)
			pkt++;
		printf("\npakets:%d:%d:%d\n",pkt,rem,len);
		if (pkt > 1){
		buff=(char*)realloc(buff,pkt*BUFF_SIZE);
		
		j=BUFF_SIZE;
		rxd = 1;
			
		while(pkt>1){
			if((num=recvfrom(sockfd,&buff[j],BUFF_SIZE,0,i->ai_addr,&i->ai_addrlen))==-1){
				printf("response not received from server\n");
				rxd = 0;
			}
			j+=BUFF_SIZE;
			pkt--;
			rxd++;
		}		
		if (pkt == 1){
			if((num=recvfrom(sockfd,&buff[j],rem,0,i->ai_addr,&i->ai_addrlen))==-1){
				printf("response not received from server\n");
				rxd = 0;
			}
		}
		if (rxd == 0)
			continue;
		}
		//printing the received output
		printf("Server response:\n%s%s",buff,EOM);
		buff=(char*)calloc(1,BUFF_SIZE);
		memset(buff,'\0',BUFF_SIZE);
		exe--;
	}
	
	free(exec_count);
	free(time_delay);
	free(command);
	free(mes);	
	freeaddrinfo(serverinfo);
	return 0;
}

