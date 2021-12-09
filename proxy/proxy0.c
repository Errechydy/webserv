// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, client_fd;
    long valread;
    struct sockaddr_in address, address2;
    int addrlen = sizeof(address);
    int addrlen2 = sizeof(address2);
    
    // Only this line has been changed. Everything is same.
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello worldffff!";
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) // address will be filled with the ip address of the client who requested the server
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        // char buffer[30000] = {0};
        // valread = recv( new_socket , buffer, 30000, 0);
        // printf("%s\n",buffer );












        // Creating socket file descriptor
        if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("In socket");
            exit(EXIT_FAILURE);
        }
        address2.sin_family = AF_INET;
        address2.sin_port = htons( 80 );   


        // address.sin_addr.s_addr = htonl((unsigned long)"13.58.131.53");
        // char* req = "GET /gamesList/2021-11-18 HTTP/1.1\r\nConnection: keep-alive\r\nAccept: text/html,application/json\r\n\r\n";

        // address.sin_addr.s_addr = htonl((unsigned long)"142.250.200.78"); // google
        char* req = "GET / HTTP/1.1\r\nConnection: keep-alive\r\nAccept: text/html,application/json\r\n\r\n";


        // if(inet_pton(AF_INET, "13.58.131.53", &address2.sin_addr) <= 0)
        if(inet_pton(AF_INET, "142.250.200.78", &address2.sin_addr) <= 0)
        {
            perror("In inet_pton");
            exit(EXIT_FAILURE);
        }


        
        memset(address2.sin_zero, '\0', sizeof address2.sin_zero);
        if ((connect(client_fd, (struct sockaddr *)&address2, sizeof(address2))<0))
        {
            perror("In connect");
            exit(EXIT_FAILURE);
        }
        send(client_fd , req , strlen(req), 0);




    //     char buffer2[300000] = {0};
    //     recv( client_fd , buffer2, 300000, 0);
    //    printf("Buffer 2 : ======> %s\n",buffer2 );

    //     unsigned int len = strlen(buffer2);

    //     printf("len = %lu \n", strlen(buffer2));


    //     buffer2[len] = '\r';
    //     buffer2[len+1] = '\n';
    //     buffer2[len+2] = '\r';
    //     buffer2[len+3] = '\n';

    //     send(new_socket , buffer2 , strlen(buffer2), MSG_OOB);
    //     close(client_fd);
        



       // write(new_socket , hello , strlen(hello));






        char buffer[30000] = {0};
        int msgsize = 0;
        size_t bytes_read;
        while((bytes_read = recv(client_fd, buffer + msgsize, sizeof(buffer) - msgsize - 1, 0)))
        {
            msgsize += bytes_read;
            if(msgsize > 30000 - 1 || (buffer[msgsize-1] == '\n' && buffer[msgsize-2] == '\r' && buffer[msgsize-3] == '\n' && buffer[msgsize-4] == '\r'))
                break;
        }

    //    printf("======> %s\n",buffer);





        int msgsentsize = 0;
        int bytes_sent = 0;
        while((bytes_sent = send(new_socket, buffer + msgsentsize, sizeof(buffer) - msgsentsize - 10, 0)))
        {
            msgsentsize += bytes_sent;
            if(msgsentsize == sizeof(buffer))
                break;
        }


        printf("------------------Hello message sent-------------------");
        // write(new_socket , hello , strlen(hello));


        close(new_socket);
    }
    close(server_fd);
    return 0;
}