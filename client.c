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
    
    //Confirmation that server and client are communicating
    puts("------------Connected to Server!!-----------\n");

    while( (valread = read(sock, buffer, 30000)) > 0){
        printf("%s\n", buffer);

        //Send Message 
        //printf("Enter a message: "); 
        //Get input instruction
        fgets(message, 2048, stdin); 
        send(sock, message, strlen(message), 0); //send to server
        
        //Server side handles all the instructions


        memset(buffer, 0, 30000);
        memset(message, 0, 2000);
    } 
    
    return 0; 
}
