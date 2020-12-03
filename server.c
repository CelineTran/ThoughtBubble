// Server opens up port 

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h> 
#include <dirent.h> 

#define PORT 8080


//char* appendCharToCharArray(char* array, char a); 

//pthread_mutex_t lock = PTHREAD_MUTEX_INTIALIZER; 

void *connection_handler(void *); 

int getDirectory(int argc, char**argv){
    //directory data type, to store the path of the directory
    DIR* dirPath;
	//data structure for directory entries
    struct dirent* dirEntry;

	//if opendir function returns as failed, then print error and exit
    if(!(dirPath=opendir(argv[1])) ) {
        printf("opendir error\n");
        exit(1);
    }
    
	//while loop to read directory and breaks when it has reached the end of path
    while(1) {
        dirEntry=readdir(dirPath);
        if(!dirEntry)
            break;
		    //print path
        printf("%s \n", dirEntry->d_name);

    }
  
  return 0;
}

int main(int argc, char const *argv[]){
    int serverfd, new_socket; long valread; 
    struct sockaddr_in address; 
    int addrlen = sizeof(address); 
    char message[100] = {0}; 

    //pthread_mutex_lock(&lock); 

    //String pointer, last portion is the body, what is being outputted
    //char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHellos world!"; 
    //char *message2 = "HTTP/1.1 200 OK\nContent-Type: text/plain\nontent-Length: 12\n\nCeline"; 


    // Create Socket file descriptor 
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Error in socket"); 
        exit(EXIT_FAILURE); 
    }

    // Identify socket
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT); 

    //Bind Socket 

    if(bind(serverfd, (struct sockaddr *)&address, sizeof(address)) <0){
        perror("Error in binding"); 
        exit(EXIT_FAILURE);
    }
    
    //Listen for Socket 
    // wait for incoming connection 
    if(listen(serverfd, 10) < 0){
        perror("Error in listening"); 
        exit(EXIT_FAILURE); 
    }
    pthread_t tid; 

    while(1){
        printf("\n---------Server Started----------\n\n"); 
        if((new_socket = accept(serverfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("Error in accpet"); 
            exit(EXIT_FAILURE); 
        }

        puts("Connection accepted"); 

        if(pthread_create(&tid, NULL, connection_handler, (void*)&new_socket) < 0){
            perror("Failed to create thread \n"); 
            exit(EXIT_FAILURE); 
        }

        pthread_join(tid, NULL); 

        puts("Handler assigned!"); 
    }   
    
    return 0; 
}

void *connection_handler(void *socket_destination){
    
    //Get socket descriptor 
    int sock = *(int*)socket_destination; 
    //char buffer[4096]; 
    //bzero(buffer, 4096); 

    //read(sock, buffer, sizeof(buffer)); 

    //printf("%s\n", buffer); 

    int read_size; 
    char *message, client_message[4096]; 

    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 4096 , 0)) > 0 )
    {
        char get[3] = "get";
        char post[4] = "post"; 
        for(int i = 0, j = 0; i < strlen(client_message); ++i){
            if(client_message[i] == get[j]){
                ++j; 
                if(j == 3){
                    char *p[2] = {"ls", "."}; 
                    getDirectory(2, p);
                    printf("GET command!\n"); 
                }
            }
            else if(client_message[i] == post[j]){
                ++j; 
                if(j == 4){
                    printf("POST command!\n"); 
                }
            }
            else{
                j -= i; 
                j = 0; 
            }
        }
        //end of string marker
		client_message[read_size] = '\0';
		
		//Send the message back to client
        write(sock , client_message , strlen(client_message));

        printf("Message Received: %s\n", client_message); 
		
		//clear the message buffer
		memset(client_message, 0, 4096);
    }

    close(sock); 
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;

}
