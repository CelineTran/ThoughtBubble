// Server opens up port 

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

int main(int argc, char const *argv[]){
    int serverfd, new_socket; long valread; 
    struct sockaddr_in address; 
    int addrlen = sizeof(address); 

    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!"; 


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

    while(1){
        printf("\n---------Waiting for connection----------\n\n"); 
        if((new_socket = accept(serverfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("Error in accpet"); 
            exit(EXIT_FAILURE); 
        }

        char buffer[30000] = {0}; 
        valread = read(new_socket, buffer, 30000); 
        printf("%s\n", buffer);
        write(new_socket, hello, strlen(hello)); 
        printf("---------Hello Message Sent---------"); 
        close(new_socket); 
    }

    return 0; 
}