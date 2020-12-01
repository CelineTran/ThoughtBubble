// Client code 

#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h> 

#define PORT 8080 

int main(int argc, char const *argv[]){
    int sock = 0; 
    long valread; 
    struct sockaddr_in serv_addr; 
    char *hello = "Hello from client"; 
    char buffer[1024] = {0}; 
    char message[100] = ""; 
    

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

/*     send(sock, hello, strlen(hello), 0);
    printf("Hello message sent\n"); 
    valread = read(sock, buffer, 1024); 
    printf("%s\n", buffer); */

     while(1){
        //Send Message 
        printf("Enter a message: "); 
        fgets(message, 100, stdin); 
        send(sock, message, strlen(message), 0);
        printf("Message sent\n"); 
        
        valread = read(sock, buffer, 1024); 
        printf("%s\n", buffer);
    } 
    
    return 0; 
}
