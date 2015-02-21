/*************************************************************
*
*       FILE NAME: doctor.c
*       
*       DESCRIPTION: This file has all the prototypes for  
*                    functions in use and the headerfiles,
*                    used for the doctor and the functions 
*                    used for its working and the main.
*                     
*                   
*       Revision History:
*       Date            Name                    Reason
*      ----------------------------------------------------
*       11/30/2014      AKSHATA SHESHAGIRI      EE450 project 
*                       NAIK                                              
*
**************************************************************/

/********************** Header Definitions *******************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXBUFLEN 1024
int main(void)
{
       
    /***************Declare and Initialize Variables***************/
    
    char p1[25],p2[25],p3[25],in1[25],in2[25],in3[25], insName[25],docbuff[MAXBUFLEN],buf[MAXBUFLEN];
    FILE* docpt;
    int sockfd,pid,rv,port,getsock_check,numbytes;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t addr_len, len;
    char ipstr[INET6_ADDRSTRLEN],ipadd[INET6_ADDRSTRLEN],MYPORT[10],DocName[25];
    struct sockaddr_in myaddrUDP;
        
    /**************************** Call Fork()  ******************************/
	pid = fork();
    if(pid==0)
    {
    	strcpy(MYPORT,"41634");
        strcpy(DocName,"Docter 1");
        /**********************   Open DOC1 file   **************************/
       memset(docbuff,0,MAXBUFLEN);
	   docpt=fopen("doc1.txt","r");
    }
    else if(pid>0)
    {
        strcpy(MYPORT,"42634");
        strcpy(DocName,"Docter 2");
        /***********************  Open DOC2 file   **************************/
       memset(docbuff,0,MAXBUFLEN);
	   docpt=fopen("doc2.txt","r");
	   
    }
    else
    {
        perror("ERROR on fork");
	    exit(1);
    }
    
    /********************** Load doc File And Store The Values  **************************/
	
    if(docpt==NULL)
	   {
		  printf("\n Could not read file. fopen() error! \n");
		  return 1;
	   }
	   if(!(fread(docbuff,1,sizeof(docbuff),docpt)))
	   {
		  printf("\n Fread error!!\n ");
		  return 1;
	   }
	   fclose(docpt);
	   sscanf(docbuff,"%s %s %s %s %s %s", in1,p1,in2,p2,in3,p3);
	
	
	/********************  Initialize struct Address for sockets  *************************/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_DGRAM;
	
	if ((rv = getaddrinfo("nunki.usc.edu", MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	/****************** loop through all the results and bind to the first we can  *******************/
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
 			perror("listener: socket");
 			continue;
 		}
 		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
 			close(sockfd);
 			perror("listener: bind");
 			continue;
 		}
 		break;
 	}
 	if (p == NULL) {
 		fprintf(stderr, "listener: failed to bind socket\n");
 		return 2;
 	}
    
   
    addr_len=sizeof(myaddrUDP);
    getsock_check=getsockname(sockfd,(struct sockaddr*)&myaddrUDP,&addr_len);
    printf("\n Phase 3: %s has static UDP port %s and IP address %s.\n", DocName, MYPORT, inet_ntoa(myaddrUDP.sin_addr));
    
	freeaddrinfo(servinfo);
while(1)
{
	//printf("listener: waiting to recvfrom...\n");
	addr_len = sizeof their_addr;
    memset(buf,0,MAXBUFLEN);
    
	/*******************Receive queryfor the Price from Patient****************************/
	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
		}
	
    
    len=sizeof their_addr; 
    getpeername(sockfd, (struct sockaddr*)&their_addr, &len);
    struct sockaddr_in *sa = (struct sockaddr_in *)&their_addr;
    port = ntohs(sa->sin_port);
    buf[numbytes]='\0';
    sscanf(buf,"%s",insName);

    printf("\n Phase 3: %s receives the request from the patient with port number %d and the insurance plan %s \n  ",DocName, port, insName);
   
    /***********************Check for the Price for the Insurance Type *****************************/
    if((strcmp(insName,in1))==0)
    {
        memset(buf,0,MAXBUFLEN);
        strcpy(buf,p1);
        if ((numbytes = sendto(sockfd,buf, strlen(buf)+1,0,(struct sockaddr *)&their_addr, addr_len)) == -1) {
            perror("doctor Error at sendto");
            exit(1);
        }
        printf("\n Phase 3: %s has sent estimated price %s$ to patient with port number %d. \n ", DocName,p1,port);
    }
    else if((strcmp(insName,in2))==0)
    {
        memset(buf,0,MAXBUFLEN);
        strcpy(buf,p2);
        if ((numbytes = sendto(sockfd,buf, strlen(buf)+1,0,(struct sockaddr *)&their_addr, addr_len)) == -1) {
            perror("doctor Error at sendto");
            exit(1);
        }
        printf("\n Phase 3: %s has sent estimated price %s$ to patient with port number %s. \n ", DocName,p2,port);
    }
    else if((strcmp(insName,in3))==0)
    {
        memset(buf,0,MAXBUFLEN);
        strcpy(buf,p3);
        if ((numbytes = sendto(sockfd,buf, strlen(buf)+1,0,(struct sockaddr *)&their_addr, addr_len)) == -1) {
            perror("doctor Error at sendto");
            exit(1);
        }
        printf("\n Phase 3: %s has sent estimated price %s to patient with port number %s. \n ", DocName,p3,port);
    }
	
    
}
	close(sockfd);
	return 0;
}


