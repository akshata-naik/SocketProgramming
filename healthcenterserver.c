/*************************************************************
*
*       FILE NAME: server.c
*       
*       DESCRIPTION: This file has all the prototypes for  
*                    functions in use and the headerfiles,
*                    used for the server and the functions 
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


#include <fcntl.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

#define BACKLOG 5 //Number Of Pending Connections The Queue Will Hold
#define MAXDATASIZE 1024
#define MAXLINES 6
#define btoa(x)	((x)?"true":"false")


int main()
{
    char Myport[] = "21634";
    int sockfd, new_fd,numbytes,yes=1,rv,leng=0,no_of_characters_read=0,z=0,l=0,Ind=0,getsock_check,port;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; 
    socklen_t sin_size,len,addr_len;
    struct sockaddr_in myaddrS;
    char ipstr[INET6_ADDRSTRLEN],buf[MAXDATASIZE],data1[50],data2[50],data3[50],userbuff[MAXDATASIZE],availbuff[MAXDATASIZE];
    FILE *user;
    char p1[25],p2[25],pwd1[25],pwd2[25],selected[10],Index[2];
    size_t length;
	FILE *avail;
    
    struct App
    {
        char TimeIndex[2];
        char day[4];
        char Time[5];
        char doc[5];
        char port[6];
        bool booked;
    };
    struct App apt[10];
		
	
	memset(availbuff, '\0',MAXDATASIZE);
	memset(userbuff, '\0', MAXDATASIZE);
	user=fopen("users.txt","r");
	if(user==NULL)
	{
		printf("\n Could not read file.fopen() error! \n");
		return 1;
	}
	if(!(fread(userbuff,1,sizeof(userbuff),user)))
	{
		printf("\n Fread error!!");
		return 1;
	}
	fclose(user);
	sscanf(userbuff, "%s %s %s %s", p1,pwd1,p2,pwd2);
       

    /********************Read from available.txt and load in struct App **********************/		
	avail=fopen("availabilities.txt","r");
	char *Appointments[10]={NULL};
	char *token,*Tempo;
	Tempo=(char*)malloc(sizeof(char)*50);
    memset(Tempo,'\0',sizeof(char)*50);
	while(1)
    {
                if(fgets(Tempo,100, avail)==NULL)
                {
                    break;
                }
                else
                {
                    
                        token = strtok(Tempo," ");
                        if( token != NULL )
                        {
                                strcpy(apt[l].TimeIndex,token);
                                token = strtok(NULL," ");
                        }

                        if( token != NULL )
                        {
                                strcpy(apt[l].day,token);
                                token = strtok(NULL," ");
                        }
                        if( token != NULL )
                        {
                                strcpy(apt[l].Time,token);
                                token = strtok(NULL," ");
                        }
                        if( token != NULL )
                        {
                                strcpy(apt[l].doc,token);
                                token = strtok(NULL," ");
                        }
                        if( token != NULL )
                        {
                                strcpy(apt[l].port,token);
                                token = strtok(NULL," ");
                        }
			     apt[l].booked=false;
                        l++;
                }
    }
        
	fclose(avail);

    /************************   Initialize Address Structs   ***********************/
	memset(buf, '\0', MAXDATASIZE);
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
    /*---START--------------------------- Reference: Beej’s guide to network programming-------------------------->*/
	if ((rv = getaddrinfo("nunki.usc.edu", Myport, &hints, &servinfo)) != 0) {
		perror("Error getting address");
		return 1;
	}	
	
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,0)) == -1) {
			perror("server: socket");
			continue;
	 	}	
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}	
		break;
	}
	if (p == NULL) {
		perror("Error: servinfo=NULL");
		return 2;
	}
    
    addr_len=sizeof(myaddrS);
    getsock_check=getsockname(sockfd,(struct sockaddr*)&myaddrS,&addr_len);

	printf("\n Phase 1:The Health Center Server has port number %s and IP address %s. \n", Myport, inet_ntoa(myaddrS.sin_addr));
	
	if ((listen(sockfd, BACKLOG)) == -1) {
		perror("listen");
		exit(1);
	}	
	//printf("\n server: waiting for connections...\n");
	
	while(1)
	 { // main accept() loop
		sin_size = sizeof (their_addr);
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}
     /*--END-------------------------- Reference: Beej’s guide to network programming-------------------------->*/
        
        /*********************   Receive Request from Patient ***************************/
		if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");	
			exit(1);
		}	                
		buf[numbytes]='\0';
		sscanf(buf,"%s %s %s",data1, data2, data3);	
		data3[strlen(data3)+1]='\0';
        
        
        /*********************************   Authentication   ************************************/
        
		memset(buf,0,MAXDATASIZE);
	   if((strcmp(data1,"authenticate"))==0)
		{	
			printf("\n Phase 1: The Health Center Server has received request from a patient with username %s and password %s\n", data2, data3);
			if((strcmp(data2,p1))==0)
			{
				if((strcmp(data3,pwd1))==0)
				{
					printf("\n Phase 1: The Health center sends the response SUCCESS to patient with username %s \n ",data2);
					if(send(new_fd, "Success",8,0) == -1)
						perror("Server send error");
				}
				else
				{
                        printf("\n Phase 1: The Health center sends the response FAILURE to patient with username %s \n ",data2);
						if(send(new_fd, "Failure",8,0) == -1)
						perror("Server send error");	
				}		
			}
			else if((strcmp(data2,p2))==0)
			{
				if((strcmp(data3,pwd2))==0)
				{
					printf("\n Phase 1: The Health center sends the response SUCCESS to patient with username %s \n",data2);
					if(send(new_fd, "Success",8,0) == -1)
                            perror("Server send error");
                }
                else
				{
                    printf("\n Phase 1: The Health center sends the response FAILURE to patient with username %s \n ",data2);
                    if(send(new_fd, "Failure",8,0) == -1)
                            perror("Server send error");
				}
			}
			else 
			{
                 printf("\n Phase 1: The Health center sends the response FAILURE to patient with username %s \n ",data2);
				 if(send(new_fd, "Failure",8,0) == -1)
                                 perror("Server send error");
                continue;
			}	
	} 
        
        /*******************  Receive Request For Appointment Booking   *********************/
	
        if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
                   perror("recv");
                   exit(1);
            }
	   buf[numbytes]='\0';
	memset(ipstr,0,sizeof(ipstr));
        len=sizeof their_addr; 
        getpeername(new_fd, (struct sockaddr*)&their_addr, &len);
           struct sockaddr_in *s = (struct sockaddr_in *)&their_addr;
           port = ntohs(s->sin_port);
           inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
       	ipstr[strlen(ipstr)+1]='\0'; 
        
        /************************** Appointment Availability   ************************/
        
	   if((strcmp(buf,"available"))==0)
	   {	
		      printf("\n Phase 2: The Health Center Server receives a request for available time slots from patient with port number %d  and IP address %s \n", port, ipstr);
           
		memset(buf,0,MAXDATASIZE);
		for(l=0;l<MAXLINES;l++)
		{
			if(apt[l].booked==true)
					continue;
			strcat(buf,apt[l].TimeIndex);
			strcat(buf," ");
			strcat(buf,apt[l].day);
			strcat(buf," ");
			strcat(buf,apt[l].Time);
			strcat(buf,"  .");
		}
		//printf("\n Data sent over buffer %s \n",buf);
           
		/****************Send Appointment list to Patient********************************/
           
        	printf("\n Phase 2: The Health Center Server sends the available time slots to the patient with username %s \n", data2);
        	if(send(new_fd, buf ,strlen(buf)+1,0) == -1)
                	     perror("Server send error");

        	memset(buf,0,MAXDATASIZE);
           
		/*************** Appointment request from patient********************************/
        	if ((numbytes = recv(new_fd,buf,MAXDATASIZE, 0)) == -1) {
                	   perror("recv");
                   		exit(1);
           	 }	
		      buf[numbytes]='\0';
		      sscanf(buf,"%s %s",selected,Index);
		      Ind=atoi(Index);
           
              printf("\n Phase 2: The Health Center Server recieves a request for appointment %d from patient with port number %d and username %s .\n ", Ind,port,data2);

           
              /*************************  Confirm/Reject Apointment Request   *****************************/
		      memset(buf,0,MAXDATASIZE);
		      if(apt[Ind-1].booked==false)
		      {
			     printf("\n Phase 2: The Health Center server confirms the following appointment %d to patient with username %s. \n", Ind, data2);
			     apt[Ind-1].booked=true;
			     strcat(buf,apt[Ind-1].doc);
			     strcat(buf," ");
			     strcat(buf,apt[Ind-1].port);
			     if(send(new_fd, buf ,strlen(buf)+1,0) == -1)
                             perror("Server send error");
		      }
		      else
		      {
			         printf("\n Phase 2: The Health Center Server rejects the following appointment %d to patient with username %s. \n ", Ind, data2);
			         strcpy(buf,"notavailable");
			         if(send(new_fd, buf ,strlen(buf)+1,0) == -1)
                                    perror("Server send error");
					
		      }	

	}
}
return 0;
}
	
