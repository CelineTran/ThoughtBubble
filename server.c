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

//function called to parse the user's input to identify specific command
//and subsequent arguments
char **get_input(char *input){
  // initialize pointer to pointer by allocating //block of memory on the heap
  char **command = malloc(8* sizeof(char *));
  
  //will identify a space " " as the separator key between commands and 
	//arguments
  char *separator = " "; 
  char *parsedInput; 
	//used for the index of command array
  int index = 0; 

  //strtok function breaks input string into multiple parts
  //separator signals where to break
  parsedInput = strtok(input, separator); 

	
  while(parsedInput != NULL){
    // Iterate through the tokens with index and subsequent increments
    command[index] = parsedInput; 
    index++; 
    
    //empty parsed
    parsedInput = strtok(NULL, separator); 
  }

  // Set final index of command to null in order to terminate
  command[index] = NULL; 
  return command; 
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

    printf("\n---------Server Started----------\n\n"); 

    while(1){
        if((new_socket = accept(serverfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("Error in accpet"); 
            exit(EXIT_FAILURE); 
        }

        puts("Connection accepted"); 

        if(pthread_create(&tid, NULL, connection_handler, (void*)&new_socket) < 0){
            perror("Failed to create thread \n"); 
            exit(EXIT_FAILURE); 
        }

        //pthread_join(tid, NULL); 

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
    char *message, *instruction, client_message[4096]; 
    char returnData[2048]; 

    char **command; 

    //Send some messages to the client
    message = "You are connected to the Thought Bubble server\nClient Commands: \nget open <file name>\nget files\npost new <file name>\npost edit <original file name>\n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 4096 , 0)) > 0 )
    {

        //end of string marker
		client_message[read_size] = '\0';

        printf("Message Received: %s\n", client_message); 

        command = get_input(client_message); 
        if(strcmp(command[0], "get") == 0){
            printf("GET command!\n"); 
        }
        else if(strcmp(command[0], "post") == 0){
            printf("POST command! \n"); 
        }
        else{
            instruction = "enter help"; 
            write(sock, instruction, strlen(instruction)); 
        }

		//Send the message back to client
        message = "Success \n"; 
        write(sock , message , strlen(message));

		
		//clear the message buffer
		memset(client_message, 0, 4096);
        free(command); 
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
