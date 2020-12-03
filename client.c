// Client code 

#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h> 
#include <dirent.h> 
#include <fcntl.h>
#include <errno.h> 

#define PORT 8080 
#define BUFFER_SIZE 1024 


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

int getFile(int argc, char **argv){

    char *buff[BUFFER_SIZE]; 
    int srcFile, targetFile, readStatus, writeStatus; 

    srcFile = open(argv[0], O_RDONLY); 
    targetFile = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR );

    if(srcFile == -1){
	    printf("\nError opening file %s errno = %d\n", argv[0], errno); 
		return(1); 
	}
	if(targetFile == -1){
		printf("\n Error opening file %s errno = %d\n", "temp.txt", errno); 
		return(1); 
	}

    while((readStatus = read(srcFile, buff, BUFFER_SIZE)) > 0){
		//temp variable to store status from write function
		writeStatus = write(targetFile, buff, readStatus); 
		//if return status from read and write functions are not equal, print error
		//message
		if(writeStatus != readStatus)
			printf("\n Error in writing data to %s\n", "temp.txt"); 
	}

    close(srcFile); 
    close(targetFile); 

    return 0; 

}

int postEdit(int argc, char **argv){

    char *buff[BUFFER_SIZE]; 
    int srcFile, targetFile, readStatus, writeStatus; 

    srcFile = open(argv[0], O_RDONLY); 
    targetFile = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR );

    if(srcFile == -1){
	    printf("\nError opening file %s errno = %d\n", argv[0], errno); 
		return(1); 
	}
	if(targetFile == -1){
		printf("\n Error opening file %s errno = %d\n", argv[1], errno); 
		return(1); 
	}

    while((readStatus = read(srcFile, buff, BUFFER_SIZE)) > 0){
		//temp variable to store status from write function
		writeStatus = write(targetFile, buff, readStatus); 
		//if return status from read and write functions are not equal, print error
		//message
		if(writeStatus != readStatus)
			printf("\n Error in writing data to %s\n", argv[1]); 
	}

    remove(argv[0]); 

    //close(srcFile); 
    close(targetFile); 

    return 0; 

}

int postFile(int argc, char **argv){
    FILE *fptr; 
    char text[2048]; 

    fptr = fopen(argv[0], "w"); 
    
    if(fptr == NULL){
        perror("Error opening file!"); 
        //exit(EXIT_FAILURE); 
    }

    fclose(fptr);

    return 0; 
}

char *getDirectory(int argc, char**argv){
    //directory data type, to store the path of the directory
    DIR* dirPath;
	//data structure for directory entries
    struct dirent* dirEntry;
    char returnDir[2048] = {'\0'}; 

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
        //strcat(returnDir, dirEntry->d_name); 
        //strcat(returnDir, "\n"); 

    }
  
  return returnDir;
}

int main(int argc, char const *argv[]){
    int sock = 0; 
    long valread; 
    struct sockaddr_in serv_addr; 
    //char *hello = "Hello from client"; 
    char buffer[30000]; 
    char message[2048]; 
    

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) <0){
        printf("\n Socket creation error \n"); 
        return -1; 
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    // Identify Address
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0){
        printf("\nInvalid address/Address not supported \n"); 
        return -1;
    }

    //Connect to Socket 
    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
        printf("\nFailed to connect\n"); 
        return -1; 
    }

    puts("------------Connected to Server!!-----------\n");

    char *instruction; 

    while( (valread = read(sock, buffer, 30000)) > 0){
        printf("%s\n", buffer);

        //Send Message 
        //printf("Enter a message: "); 
        fgets(message, 2048, stdin); 
        send(sock, message, strlen(message), 0);
        printf("Message sent\n\n"); 

        char **command; 
    
        command = get_input(message); 
        if(strcmp(command[0], "get") == 0){
            if(!command[1]){
                instruction = "Please enter a command after 'get' "; 
                write(sock, instruction, strlen(instruction)); 
            }
            else if(strcmp(command[1], "open") == 0){
                printf("Opening file\n"); 
                char *p[1] = {command[2]}; 
                getFile(1,p); 
            }
            else if(command[1]){
                char *p[2] = {"ls", "."}; 
                getDirectory(2, p);
            }
            printf("\n"); 
        }
        else if(strcmp(command[0], "post") == 0){
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

            printf("POST command! \n"); 
        }
        else{
            instruction = "enter help"; 
            printf("Help"); 
            //write(sock, instruction, strlen(instruction)); 
        }

        memset(buffer, 0, 30000);
        memset(message, 0, 2000);
    } 
    
    return 0; 
}
