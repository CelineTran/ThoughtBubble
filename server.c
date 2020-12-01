// Server opens up port 

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080


//char* appendCharToCharArray(char* array, char a); 

int main(int argc, char const *argv[]){
    int serverfd, new_socket; long valread; 
    struct sockaddr_in address; 
    int addrlen = sizeof(address); 

    int conn; 
    char message[100] = {0};  


    //String pointer, last portion is the body, what is being outputted
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHellos world!"; 
    //char *message1 = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\n"; 

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

    while((new_socket = accept(serverfd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) > 0){
        int pid; 
        if((pid = fork()) == 0){
            while(recv(new_socket, message, 100, 0) >0){
                printf("Message Received: %s\n", message); 
                printf("PID: %i",pid); 
                //valread = read(new_socket, message, 30000); 
                //message1 = appendCharToCharArray(message1, message); 
                write(new_socket, message, strlen(message)); 
                //break; 
            }
            //close(new_socket); 
            exit(0); 
        }
    } 

/*      while(1){
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
    }  */

    return 0; 
}

/* char* appendCharToCharArray(char* array, char a)
{
    size_t len = strlen(array);

    char* ret = new char[len+2];

    strcpy(ret, array);    
    ret[len] = a;
    ret[len+1] = '\0';

    return ret;
}
 */