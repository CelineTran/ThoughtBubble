// Server opens up port 

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h> 
#include <dirent.h> 
#include <fcntl.h>
#include <errno.h> 

#define PORT 8080
//default PORT connection


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

//getFile opens source file, copies data into a newly create text file called "temp.txt" 
char *getFile(int argc, char **argv){

    char *buff[2048]; 
    char *output = "File saved to temp.txt\n"; 

    int srcFile, targetFile, readStatus, writeStatus; 

    //open source file as read only and return value to srcFile based on success
    srcFile = open(argv[0], O_RDONLY); 

     //create and open target file with following permissions
     //write-only, created file, truncated, read-user, write-user
     // default target file is temp.txt
    targetFile = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR );
	
	
    //if srcFile contains failed status, print error message 
    if(srcFile == -1){
	    printf("\nError opening file %s errno = %d\n", argv[0], errno); 
    }
    //if targetFile contains failed status, print error message
    if(targetFile == -1){
	   printf("\n Error opening file %s errno = %d\n", "temp.txt", errno); 
    }

    //while loop as long as nbread is > 0, continue reading srcFile
    while((readStatus = read(srcFile, buff, 2048)) > 0){
		//temp variable to store status from write function
		writeStatus = write(targetFile, buff, readStatus); 
	    
		//if return status from read and write functions are not equal, print error
		//message
		if(writeStatus != readStatus)
			printf("\n Error in writing data to %s\n", "temp.txt"); 
	}

    // close out of both our source and target files
    close(srcFile); 
    close(targetFile); 

    //outputs the success message to the client
    return output; 

}

//postEdit opens the default source file of "temp.txt" and copies it to the desired source file
// also deletes the "temp.txt" file
int postEdit(int argc, char **argv){

    char *buff[2048]; 
    int srcFile, targetFile, readStatus, writeStatus; 

    //open source file as read only and return value to srcFile based on success
    //default source file is "temp.txt"
    srcFile = open(argv[0], O_RDONLY); 

    //create and open target file with following permissions
     //write-only, created file, truncated, read-user, write-user
    targetFile = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR );

    //if srcFile contains failed status, print error message 
    if(srcFile == -1){
	    printf("\nError opening file %s errno = %d\n", argv[0], errno); 
    }

    //if targetFile contains failed status, print error message
    if(targetFile == -1){
	printf("\n Error opening file %s errno = %d\n", argv[1], errno); 
    }

    while((readStatus = read(srcFile, buff, 2048)) > 0){
		//temp variable to store status from write function
		writeStatus = write(targetFile, buff, readStatus); 
		//if return status from read and write functions are not equal, print error
		//message
		if(writeStatus != readStatus)
			printf("\n Error in writing data to %s\n", argv[1]); 
	}
    
    // removes/deletes the "temp.txt" file
    remove(argv[0]); 

    //close target file
    close(targetFile); 

    return 0; 

}

//postFile creates a new text file based on inputted name
int postFile(int argc, char **argv){
    FILE *fptr; 

    //char *parsedInput; 
    //parsedInput = strtok(argv[0], ".");
    //strcat(parsedInput, ".txt");  

    fptr = fopen(argv[0], "w"); //creates file with writing permissions
    
    if(fptr == NULL){
        perror("Error opening file!"); 
        //exit(EXIT_FAILURE); 
    }

    fclose(fptr);

    return 0; 
}

//this function lists the contents of the present working directory
char *getDirectory(int argc, char**argv){
    //directory data type, to store the path of the directory
    DIR* dirPath;
	//data structure for directory entries
    struct dirent* dirEntry;
    char *returnDir; 

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
        //printf("%s \n", dirEntry->d_name);
        strcat(returnDir, dirEntry->d_name); 
        strcat(returnDir, "\n"); 

    }
  
  return returnDir; // returns the directory in a string format
}

//main method to create the socket
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

    //keep the socket open 
    while(1){
        if((new_socket = accept(serverfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("Error in accpet"); 
            exit(EXIT_FAILURE); 
        }

        puts("Connection accepted"); 

	//creates a new thread for each new client connection
        if(pthread_create(&tid, NULL, connection_handler, (void*)&new_socket) < 0){
            perror("Failed to create thread \n"); 
            exit(EXIT_FAILURE); 
        }

        //pthread_join(tid, NULL); 

        puts("Handler assigned!"); 
    }   
    
    return 0; 
}

//handles each client connection
void *connection_handler(void *socket_destination){
    
    //Get socket descriptor 
    int sock = *(int*)socket_destination; 
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

        printf("Message Received: %s", client_message); 

        char **command; 
    
	// logical statements that check for get or post request and runs the appropriate function based on client instructions
	// writes the appropriate output on the cliet side based on the instruction  
        command = get_input(client_message); 
        if(strcmp(command[0], "get") == 0){
            printf("GET request\n"); 
            if(!command[1]){
                instruction = "Please enter a command after 'get' "; 
                write(sock, instruction, strlen(instruction)); 
            }
            else if(strcmp(command[1], "open") == 0){
                printf("Opening file\n"); 
                char *p[1] = {command[2]};
                //getFile(&sock, 1, p);  
                strcat(returnData, getFile(1,p));
                write(sock, returnData, strlen(returnData));  
            }
            else if(command[1]){
                char *p[2] = {"ls", "."}; 
                strcat(returnData, getDirectory(2, p));
                write(sock, returnData, strlen(returnData)); 
            }
            printf("\n"); 
        }
        else if(strcmp(command[0], "post") == 0){
            printf("POST request\n"); 

            if(!command[1]){
                instruction = "Please enter a command after 'post' "; 
                write(sock, instruction, strlen(instruction)); 
            }
            else if(strcmp(command[1], "new") == 0){
                char *p[1] = {command[2]}; 
                postFile(1,p); 
            }
            else if(strcmp(command[1], "edit") == 0){
                char *p[2] = {"temp.txt", command[2]}; 
                postEdit(2,p); 
            }
            printf("\n"); 

        }
        else{
            instruction = "enter help"; 
            printf("Help"); 
            //write(sock, instruction, strlen(instruction)); 
        }

		//Send the message back to client
        message = "\nSuccess \n"; 
        write(sock , message , strlen(message));

		
		//clear the message buffer
		memset(client_message, 0, 4096);
        memset(returnData, 0, 2048); 
        //free(command); 
    }

    
     // outputs on the server side if the client disconnects
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        puts("Client disconnected");
    }

    close(sock); 
         
    return 0;

}
