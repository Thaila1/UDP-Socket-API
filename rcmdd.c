/***UDP ITERATIVE ARCHITECTURE in C *****/
		/*** @Thaila Annamalai ***/
		

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>


#define BUFF_SIZE 4096
#define EOM "------------------------\n"


int main(int argc,char* argv[]){
	//checks for proper command line arguments
	if(argc!=2){
		printf("USAGE:./server portnumber\n");
		exit(1);
	}
	
	//declaring variables 
	int sockfd,ginfo,num,sen,exec_count,time_delay,dSize,k,j=0,rem;
	char buff[2048],*timestamp,len[3],*command,*timestamp1,lengStr[3];
	char *res, *mes, *op;
	int lSize;
	time_t gtime;
	char clientIP[INET_ADDRSTRLEN];
	struct addrinfo hints,*serverinfo,*i;
	struct sockaddr_storage client_addr;
	
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	socklen_t addr_len;
	unsigned short port;
	int pkts = 0;
	
	//gets the information about the server port 
	if((ginfo=getaddrinfo(NULL,argv[1],&hints,&serverinfo))!=0){
		printf("Error in retrieving information\n");
		exit(1);
	}
	
	//opens the socket
	for(i=serverinfo;i!=NULL;i=i->ai_next){
			
		if((sockfd=socket(i->ai_family,i->ai_socktype,i->ai_protocol))==-1){	
			continue;
		}
		
		if(bind(sockfd,i->ai_addr,i->ai_addrlen)==-1){
			close(sockfd);
			continue;	
		}
		break;
		
	}
	
	if(i==NULL){
		printf("could not open socket\n");
		exit(1);
	}
	
	
	addr_len=sizeof(client_addr);
	
	while(1){
		printf("Waiting for connection...\n\n");
		memset(buff,'\0',BUFF_SIZE);
		//receives the data from the client
		if((num=recvfrom(sockfd,buff,1023,0,(struct sockaddr *)&client_addr,&addr_len))==-1){
			printf("could not receive from client\n");
			exit(1);
		}
		
		//gets the client port information
		inet_ntop(client_addr.ss_family,&((struct sockaddr_in*)&client_addr)->sin_addr,clientIP,sizeof(clientIP));
		port = ((struct sockaddr_in*)&client_addr)->sin_port;
		
		time(&gtime);
		timestamp=ctime(&gtime);
		
		printf("Status:Connected to the client\n");
		printf("%s",timestamp);
		printf("Received data from client: %s:%u\n",clientIP,port);
		
		
		//seperates the information
		
		//size of the string received
		memcpy(len,buff,3);
		dSize=atoi(len);
		
		//spliting the exec count
		memset(len,'0',3);
		memcpy(len,&buff[3],3);
		exec_count=atoi(len);
		
		//splitting the time delay
		memset(len,'0',3);
		memcpy(len,&buff[6],3);
		time_delay=atoi(len);
		
		//splitting the command
		command = (char*)calloc(sizeof(char),dSize-9);
		memset(command,'\0',dSize-9);
		memcpy(command,&buff[9],dSize-9);
		
		//printf("The command received from the client is:%s\n",command);
		printf("Command : %s\n",command);
		printf("Count: %d\n",exec_count);
		printf("Delay: %d\n\n",time_delay);
		
		//executing the command	
		for(k=0;k<exec_count;k++){
			res=(char*)calloc(BUFF_SIZE,sizeof(char));
			time(&gtime);
			FILE *fp=popen(command,"r");
			timestamp1=ctime(&gtime);
			j = 0;
			if(fp==NULL){
				printf("error in executing the command");
				break;
			}
			op = (char*)calloc(sizeof(char),1);
			memset(op,'\0',1);
			while(fgets(res,sizeof(res),fp)){
				int op_len = 1+strlen(op);
				op = (char*)realloc(op,op_len+strlen(res));	
				strcat(op,res);
					
			}	
			pclose(fp);
			lSize = strlen(op);
			long mes_size = lSize + 5 + strlen(timestamp1);
			
			//forming packets to accomodate the data to be sent
			pkts = (mes_size) / BUFF_SIZE;
			if ((rem = (mes_size)%BUFF_SIZE )!= 0)
				pkts++;
			
			//constructing a message to send
			sprintf(lengStr,"%.3d",lSize);
			
			mes=(char*)calloc(sizeof(char),pkts*BUFF_SIZE);
			memset(mes,'\0',mes_size);			
			strcpy(mes,lengStr);
			strcat(mes," ");
			strcat(mes,timestamp1);
			strcat(mes,"\n");
			strcat(mes,op);
			printf("Execution count: %d\n",k+1);
			printf("time is: %s\n",timestamp1);
			printf("Output:\n%s\n",op);
			printf("The message to be sent:\n%s",mes);
				
			while(pkts>1){			
				//sending the data to the client
				if((sen=sendto(sockfd,&mes[j],BUFF_SIZE,0,(struct sockaddr *)&client_addr,addr_len))==-1){
					printf("packet not sent\n");
					close(sockfd);
					exit(1);
				}
				j += BUFF_SIZE;
				pkts--;		
			}
			if (pkts == 1){
				if((sen=sendto(sockfd,&mes[j],rem,0,(struct sockaddr *)&client_addr,addr_len))==-1){
					printf("packet not sent\n");
					close(sockfd);
					exit(1);
				}
			}
			if (k != exec_count-1)
				sleep(time_delay);	
			if (mes != NULL){
				free(mes);
				mes = NULL;
			}			
		}	
		
		if (mes != NULL){
			free(mes);
			mes = NULL;
		}
		if (res != NULL){
			free(res);
			res = NULL;
		}
		if (op != NULL){
			free(op);
			op = NULL;
		}
		printf("%s",EOM);
	}
	
	close(sockfd);
	return 0;
		
}


