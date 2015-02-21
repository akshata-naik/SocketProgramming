/*************************************************************
*
*       FILE NAME: patient2.c
*       
*       DESCRIPTION: This file has all the prototypes for  
*                    functions in use and the headerfiles,
*                    used for the patient and the functions 
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
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define Myport "21634" // the port client will be connecting to
#define MAXDATASIZE 100
#define MSGSIZE 1024
#define MAXAPP 6 
#define MAXBUFLEN 1024

/*************************************************************
*
*       FUNCTION NAME: main
*
*       DESCRIPTION:The patient-2 sends the authentication 
*		    details to the health center server. On successful 
*       	    authentication it gets the list of available             
*       	    appointments and books one. Later it contacts
*		    the concerned doc to request for the cost 
*		    depending on the insurance he is using.             
*
*       RETURNS: NONE
*
*************************************************************/


int main()
 {	
    FILE* fp1,*inpt;
    int i=0,j=0,k=0, flag=0,l=0,r,z,bitsread=0,IndexList[10],numbytes,sockfd,rv,port;
    static int Ind=0,lines=0;
    socklen_t len,addr_len=0;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in myaddr,myaddrTCP,myaddrA;
    struct sockaddr_storage their_addr;
    struct sockaddr_in *ipv4D;
    void *d_addr;
    struct  in_addr **addr_list;
    char ipstr[INET6_ADDRSTRLEN],bufip[INET6_ADDRSTRLEN],sdd[INET6_ADDRSTRLEN];
	int sd,fd, patfd,getsock_check1,getsock_check2;
	char* token;
    char *msg;
    char databuff1[MSGSIZE], data1[50], data2[50],TimeIndex[10], docport[25],docname[25], insbuff[MSGSIZE],first[6];
	msg=(char*)malloc(sizeof(char)*MSGSIZE);
	memset(msg, '\0', MSGSIZE);
    memset(databuff1, '\0', MSGSIZE);
    memset(first,'\0',sizeof(char)*6);
    
    /*******************   Load Patient1 Data Into Buffer   ***********************/
    fp1=fopen("patient2.txt","r");
	if(fp1==NULL)
	{
		printf("\n Could not read file. fopen() Error!\n");
		return 1;
	}
	if(!(fread(databuff1,1,sizeof(databuff1),fp1)))
	{
		printf("\n Fread Error!!");
		return 1;
	}
	sscanf(databuff1, "%s %s",data1, data2);
    
    /************************  Patient TCP SOCKET  ****************************/
    
    if ((patfd=socket(AF_INET, SOCK_STREAM, 0))==-1)
		              printf("socket not created\n");

	   /* bind it to all local addresses and pick any port number */

	   memset((char *)&myaddrTCP, 0, sizeof(myaddrTCP));
	   myaddrTCP.sin_family = AF_INET;
	   myaddrTCP.sin_addr.s_addr = htonl(INADDR_ANY);
	   

	   if (bind(patfd, (struct sockaddr *)&myaddrTCP, sizeof(myaddrTCP)) < 0) 
       {
		  perror("bind failed");
		  return 0;
	   }       
    
    
    /****************   HEALTH CENTER SERVER TCP SOCKET   ******************/
    
    /*---START------------------------- Reference Beej’s guide to network programming--------------------------->*/
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
		

	if ((rv = getaddrinfo("nunki.usc.edu", Myport, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
		}	
	//loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
			perror("client:server-socket");
			continue;
			}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client:server-connect");
			continue;
			}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
		}
    
	/*---END ------------------------- Reference Beej’s guide to network programming--------------------------->*/
    
    strcpy(msg,"authenticate ");
	strcat(msg,databuff1);
    socklen_t len22 = sizeof(myaddrTCP);
    addr_len=sizeof(myaddrA);
    getsock_check1=getsockname(sockfd,(struct sockaddr*)&myaddrA,&addr_len);
    
    
    printf("\n Phase 1: Patient 2 has port number %d and IP address %s. \n",ntohs(myaddrA.sin_port),inet_ntoa(myaddrA.sin_addr));
	freeaddrinfo(servinfo);
	if (send(sockfd, msg, strlen(msg), 0) == -1)
		perror("send");
	
	printf("\n Phase 1: Authentication request from Patient2 with username %s and password %s has been sent to the Health Center Server.\n ", data1, data2);
    
    /*********************   Send Authentication Request  ************************/
    
	memset(msg,0,MSGSIZE);
		if ((bitsread = recv(sockfd,msg,MSGSIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

    /**************************   Check Authentication Result *****************************/
	msg[bitsread]='\0';
	if((strcmp(msg,"Success"))==0)
	{
		printf("\n Phase 1: Patient 2 authentication result:%s .\n ",msg);
		printf("\n Phase 1: End of Phase 1 for Patient2 \n ");
		memset(msg,0,MSGSIZE);
		/***************Send Availability Request*******************/
		if (send(sockfd,"available",10, 0) == -1)
    		            perror("send");

		/*************Receive Available appointment list*******************/
		if ((bitsread = recv(sockfd, msg, MSGSIZE-1, 0)) == -1) {
			   perror("recv");
				exit(1);}
        
        if((strcmp(msg,""))!=0)
        {
		msg[bitsread]='\0';
		token=strtok(msg,".");
		printf("\n Phase 2: The following appointments are available for %s:\n ", data1);
		l=0;
		while( token != NULL )
        	{	
                lines++;
			     printf("\n  %s\n",token);
                sscanf( token, "%s",first);
		        IndexList[Ind++]=atoi(first);
                token = strtok(NULL,".");
      		}	
		memset(msg,0,MSGSIZE);
        
		/****************Enter the prefered appointment and send *********************/
		      
        do
		{
		/* Ask the user to enter one of the available appointments */
			printf("\n Please enter the preferred appointment index and press enter:\n");
		/* check for condition where user enters no input */
			if(scanf("%s",&TimeIndex))
			{
				for(r=0;r<lines;r++)
				{
		/* User is forced to enter the right input until he fails */
					if(IndexList[r]==(atoi(TimeIndex)))
						flag=1;
				}
			}	
			else
				while(getchar() != '\n');//check for input if characters
		}while(flag!=1);
        
        strcat(msg,"selection ");
		strcat(msg,TimeIndex);
        
		if (send(sockfd,msg,strlen(msg)+1,0) == -1)
                       	    perror("send");
	
		memset(msg,0,MSGSIZE);

		/**************Receive confirmation on appointment*********************/
		if ((bitsread = recv(sockfd, msg, MSGSIZE-1, 0)) == -1) {
                           perror("recv");
			   exit(1);
        }
        msg[bitsread]='\0';
		if((strcmp(msg,"notavailable"))!=0)
		{
			sscanf(msg, "%s %s", docname, docport);
			printf("\n Phase 2: The requested appointment is available and reserved to Patient2. The assigned doctor port number is %s. \n",docport);
		}
		else
		{
			printf("\n Phase 2: The requested appointment from Patient 2 is not available. Exiting...");
            close(sockfd);
			exit(1);
		}
        
        /************************   If Appointment Successful Contact Doctor *****************************/
        int sockfd,fd;
        struct addrinfo hints, *servinfo, *p;
        struct sockaddr_in myaddr;
        int rv;
        int numbytes;
        socklen_t addr_len;
        struct sockaddr_storage their_addr;
        struct sockaddr_in remaddr;
        int slen=sizeof(remaddr);
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;
        memset(msg,0,MSGSIZE);
        
        /**************Load Insurance plan for patient 1*****************/
       memset(insbuff, '\0', MSGSIZE);
       inpt=fopen("patient1insurance.txt","r");
        if(inpt==NULL)
	   {
		      printf("\n Could not read file. fopen() Error!\n");
		  return 1;
	   }
	   if(!(fread(insbuff,1,sizeof(insbuff),inpt)))
	   {
		  printf("\n Fread Error!!");
		  return 1;
	   }
        
        /************************ Patient UDP socket  *********************/
        
        if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		              printf("socket not created\n");

	   /* bind it to all local addresses and pick any port number */

	   memset((char *)&myaddr, 0, sizeof(myaddr));
	   myaddr.sin_family = AF_INET;
	   myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	   

	   if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) 
       {
		  perror("bind failed");
		  return 0;
	   }       
        
        
        /*********************   Doctor to connect to *******************/
        /*---START------------------------- Reference Beej’s guide to network programming--------------------------->*/
        
        if ((rv = getaddrinfo("nunki.usc.edu",docport, &hints, &servinfo)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                return 1;
            }
        // loop through all the results and make a socket
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
                    perror("talker: socket");
                    continue;
                }
            break;
            }
        if (p == NULL) {
            fprintf(stderr, "talker: failed to bind socket\n");
            return 2;
        }
        
        /*---END ------------------------- Reference Beej’s guide to network programming--------------------------->*/
        memset(&myaddrA, 0, sizeof myaddrA);
        addr_len=sizeof(myaddrA);
        getsock_check2=getsockname(fd,(struct sockaddr*)&myaddrA,&addr_len);
            
            
        printf(" \n Phase 3: Patient 2 has dynamic UDP port number %d and IP address ", ntohl(myaddrA.sin_port) );
        struct hostent *lh=gethostbyname("nunki.usc.edu");
        addr_list = (struct in_addr **)lh->h_addr_list;
	       for(z = 0; addr_list[z] != NULL; z++) 
	       {
        	   printf("%s. \n", inet_ntoa(*addr_list[z]));// Ip address  
	       }	
         
            
        /***************Send request for Insurance Price details*******************/
        strcpy(msg,insbuff); 
	
        if ((numbytes = sendto(fd, msg, strlen(msg)+1, 0, p->ai_addr, p->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        msg[numbytes-2]='\0';
        ipv4D =(struct sockaddr_in *)p->ai_addr;
        d_addr =  &(ipv4D->sin_addr);
        inet_ntop(p->ai_family, d_addr, sdd, sizeof sdd);
        printf("\n Phase 3: The cost estimation request from Patient 2 with insurance plan %s has been sent to the doctor with port number %s and IP address %s. \n ", msg, docport, sdd );
       // freeaddrinfo(servinfo);
        
        /*****************Receive Insurance Price from Doctor*********************/
        memset(msg,0,MSGSIZE);
        if ((numbytes = recvfrom(fd, msg, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
		}
	msg[numbytes]='\0';
	//printf("%d", numbytes);
        printf("\n Phase 3: Patient 2 receives %s$ estimation cost from doctor with port number %s and IP address %s. \n ", msg, docport, sdd);
	printf("\n Phase 3: End of Phase 3 for Patient 2.\n");
	   
        freeaddrinfo(servinfo);
        close(sockfd);
	}
    else
    {
        close(sockfd);
        exit(1);
    }
}
	else
	{
		printf("Phase 1: Patient 1 authentication result:%s \n",msg);	
		close(sockfd);
        exit(0);
	}
 


	return 0;
}
	
