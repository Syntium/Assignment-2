#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <errno.h>
#include <sys/wait.h>

void error(const char *msg){
    perror(msg);
    exit(0);
}

void downloadFile(int sockfd){	
	printf("Downloading file from Server... ");
	
	int n;
	int buflen;

	
	char revBuff[256];
	char dir[256] = "/home/";
	char hostname[256];
	gethostname(hostname, 255);
	char file[256] = "/Client/";
	strcat(dir, hostname);
	strcat(dir, file);
	printf("\nPath: %s", dir);
	
	
	struct stat st = {0};
	if(stat(dir, &st) == -1){
	  mkdir(dir, 0700);
	}

	
	char tempo[256];
	bzero(tempo,256);
	n = read(sockfd, (char*)&buflen, sizeof(buflen));
	if (n < 0) error("Cannot read from socket");
	buflen = htonl(buflen);
	n = read(sockfd,tempo,buflen);
	if (n < 0) error("Cannot read from socket");
	printf("\nFile Available: \n");
	printf("%s", tempo);

	printf("Enter the file name to be downloaded: ");
	char selectFile[256];
	bzero(selectFile,256);
	fgets(selectFile,255,stdin);
    	char input[256];
	
	
	int datalen = strlen(selectFile);
	int tmp = htonl(datalen);
	n = write(sockfd, (char*)&tmp, sizeof(tmp));
	if(n < 0) error("Cannot write to socket");
	n = write(sockfd,selectFile,datalen);
	if (n < 0) error("Cannot write to socket");
	
	char filename[256];
	printf("Save the file as: ");
	fgets(filename, 256, stdin);

	if(filename != NULL){
		strcat(dir, filename);	
		printf("File location: %s", dir);

		FILE *fr = fopen(dir, "ab");
		if(fr == NULL){
		  printf("File cannot be accessed");
		  perror("fopen");
		  exit(0);
		}
		else{	
		  bzero(revBuff, 256);
		  int fr_block_sz = 0;
		  while((fr_block_sz = recv(sockfd, revBuff, 256, 0)) > 0){
		  	int write_sz = fwrite(revBuff, sizeof(char), fr_block_sz, fr);
			if(write_sz < fr_block_sz){
			  error("File cannot be write on server.\n");
			}
			bzero(revBuff, 256);
			if(fr_block_sz == 0 || fr_block_sz != 256){
			  break;			
			}
		  }
		  printf("\nFile successfully downloaded ");
		  fclose(fr);
		}
	}
	else{
		printf("\nFilename cannot be empty");		
		printf("\nPlease try again later");
		exit(0);
	}
}

void createFile(int sockfd){	
  	printf("Creating...");
	
	
	char content[256];
	char dir[256] = "/home/";
	char hostname[256];
	gethostname(hostname, 255);
	char file[256] = "/Client/";
	strcat(dir, hostname);
	strcat(dir, file);
	
	printf("\nPath: %s", dir);
	
	
	struct stat st = {0};
	if(stat(dir, &st) == -1){
	  mkdir(dir, 0700);
	}
	
	
	char filename[256];
	printf("\nEnter the file name: ");
	fgets(filename, 256, stdin);
	
	if(filename != NULL){
		strcat(dir, filename);
		printf("File location: %s", dir);
		
		FILE *fp;
		fp = fopen(dir, "w+");
		if(fp == NULL){
		  printf("\nFile cannot be created");
		  perror("fopen");	
		  exit(0);	
		}
		else{	
		  printf("Insert the content here: ");
		  fgets(content, 256, stdin);
		  printf("Content: %s", content);
		 
		  fprintf(fp, "%s", content);	
		  fclose(fp);
		  printf("\nFile successfully created!");
		}
	}
	else{
		printf("\nFilename cannot be empty");		
		printf("\nPlease try again later");
		exit(0);
	}
}



void deleteFile(int sockfd){	
	printf("Deleting a file...");
	
	
	char content[256];
	char dir[256] = "/home/";
	char hostname[256];
	gethostname(hostname, 255);
	char file[256] = "/Client/";
	strcat(dir, hostname);
	strcat(dir, file);
	printf("\nPath: %s", dir);
	
		
	struct stat st = {0};
	if(stat(dir, &st) == -1){
	  mkdir(dir, 0700);
	}

	
	printf("\nFile available: \n");
	DIR *directory;
	struct dirent *ent;
	if((directory = opendir(dir)) != NULL){
	  while((ent = readdir(directory)) != NULL){
		printf("%s", ent->d_name);
		printf(" ");
	  }
	  closedir(directory);
	}
	else{
	  perror("ERROR");
	  exit(0);
	}

	
	char filename[256];
	printf("\nEnter the file name to delete: ");
	fgets(filename, 256, stdin);

	//printf("\nPath2: %s", filename);
	if(filename != NULL){

		strcat(dir, filename);
		FILE *fp;
		
		
		fp = fopen(dir, "r");
		if(fp == NULL){
		  printf("\nFile cannot be found\n");
		  perror("fopen");	
		  exit(0);	
		}
		else{	
		  int status = remove(dir);
		  if(status == 0){
			printf("\nFile successfully deleted !");
			fclose(fp);
		  }else{
			printf("\nFile cannot be deleted");
			exit(0);
		  }
		}
	}
}

void sendFile(int sockfd){	

	printf("Sending file");
	char buff[256];
	int n;
	
	
	char dir[256] = "/home/";
	char hostname[256];
	gethostname(hostname, 255);
	char file[256] = "/Client/";
	strcat(dir, hostname);
	strcat(dir, file);
	printf("\nPath: %s", dir);
	
	
	struct stat st = {0};
	if(stat(dir, &st) == -1){
	  mkdir(dir, 0700);
	}
	
	
	printf("\nFile available: \n");
	DIR *directory;
	struct dirent *ent;
	if((directory = opendir(dir)) != NULL){
	  while((ent = readdir(directory)) != NULL){
		printf("%s", ent->d_name);
	  }
	  closedir(directory);
	}
	else{
	  perror("ERROR");
	  exit(0);
	}
	
	
	char tempo[256];
	printf("\nEnter the file name to be sent: ");
	fgets(tempo, 256, stdin);
	char filename[256];
	strcpy(filename, tempo); 
	
	if(filename != NULL){

		
		int datalen = strlen(tempo);
		int tmp = htonl(datalen);
		n = write(sockfd, (char*)&tmp, sizeof(tmp));
		if(n < 0) error("Cannot write to socket");
		n = write(sockfd,tempo,datalen);
		if (n < 0) error("Cannot write to socket");
	
		char split[2] = "\n";
	 	strtok(tempo, split);

		strcat(dir, filename);
		printf("Sending %s to Server... ", tempo);
		printf("\nDirectory: %s", dir);
	
		FILE *fs = fopen(dir, "rb");	
		if(fs == NULL){
		  printf("\nFile not available.\n");
		  perror("fopen");
		  exit(0);
		}
		else{	
		  bzero(buff, 256);
		  int fs_block_sz;
		  while((fs_block_sz = fread(buff, sizeof(char), 256, fs)) > 0){
		    if(send(sockfd, buff, fs_block_sz, 0) < 0){
			fprintf(stderr, "Failed to send file. %d", errno);
			break;
		    }
		    bzero(buff, 256);
		  }
		  printf("\nFile sent successfully!\n");
		  fclose(fs);
		}
	}
	else{
		printf("\nFilename cannot be empty");		
		printf("\nPlease try again later");
		exit(0);
	}
		
}


int main(int argc, char *argv[])	
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("Socket cannot be opened");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"Host does not exist\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("\n=================================");
    printf("\n\tStatus: Connected");
    printf("\n=================================");

   


    int count = 0;
    while(count == 0){	
	int proceed = 0;
	printf("\n\n1.Add new file \n2.Download file \n3.Send file to server \n4.Delete file \n5.Exit\n ");
	printf("\nPlease enter your choice: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
    	char input[256];
	strcpy(input, buffer);
	
	int datalen = strlen(buffer);
	int tmp = htonl(datalen);
	n = write(sockfd, (char*)&tmp, sizeof(tmp));
	if(n < 0) error("Cannot write to socket");
	n = write(sockfd,buffer,datalen);
	if (n < 0) error("Cannot write to socket");
	

	if((strcmp(input, "1\n")) == 0){	
	   createFile(sockfd);
	   count = 0;
	}
	else if((strcmp(input, "2\n")) == 0){	
	   downloadFile(sockfd);
	   count = 0;
	}
	else if((strcmp(input, "3\n")) == 0){	
	   sendFile(sockfd);
	   count = 0;
	}
	else if((strcmp(input, "4\n")) == 0){	
	   deleteFile(sockfd);
	   count = 0;
	}
	else if((strcmp(input, "5\n")) == 0){	
	   count = 1;
	   proceed = 1;
	}
	else{
	   printf("\nInput error!");	
	   count = 0;
	}
    }
	
    close(sockfd);
    printf("\nDisconnected from the Server.\n\n");
    return 0;
}

