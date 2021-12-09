// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>



#define PORT 8080

struct Client_socket {
    int fd;
    int reading;
    int writing;
};

static std::vector<pollfd> poll_fds;
static std::map<int, Client_socket> clients;

int set_server()
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    fcntl(server_fd, F_SETFL, O_NONBLOCK);
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

    return (server_fd);
}

int new_client(int server_fd)
{
    int client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
    {
        perror("In accept");
        exit(EXIT_FAILURE);
    }
    fcntl(client_socket, F_SETFL, O_NONBLOCK);
    return (client_socket);
}

std::string client_request(int client_socket)
{
    // std::string req = "NULL";
    // char clientRequest[300000] = {0};
    // int clientmsgsize = 0;
    // size_t client_bytes_read;
    // while((client_bytes_read = recv(client_socket, clientRequest + clientmsgsize, sizeof(clientRequest) - clientmsgsize - 1, 0)))
    // {
    //     // printf("---> %zu\n", client_bytes_read);
    //     clientmsgsize += client_bytes_read;
    //     if(clientmsgsize > 300000 - 1 || (clientRequest[clientmsgsize-1] == '\n' && clientRequest[clientmsgsize-2] == '\r' && clientRequest[clientmsgsize-3] == '\n' && clientRequest[clientmsgsize-4] == '\r'))
    //         break;
    // }
    // printf("requ: %s\n",clientRequest );
    

    clients[client_socket].reading = 1;

    std::string req = "";
    char clientRequest[300000] = {0};
    int clientmsgsize = 0;
    ssize_t client_bytes_read;
    while((client_bytes_read = recv(client_socket, clientRequest + clientmsgsize, sizeof(clientRequest) - clientmsgsize - 1, 0)) > 0)
    {
        // if(client_bytes_read == 0)
        //     break;
       // printf("---> %zu\n", client_bytes_read);
        clientmsgsize += client_bytes_read;
        if(clientmsgsize > 300000 - 1 || (clientRequest[clientmsgsize-1] == '\n' && clientRequest[clientmsgsize-2] == '\r' && clientRequest[clientmsgsize-3] == '\n' && clientRequest[clientmsgsize-4] == '\r'))
            break;
    }
    if(client_bytes_read == 0)
        printf("Client closed the connection \n");
    else if(client_bytes_read == -1)
        printf("Recv : an error occured \n");

    clients[client_socket].reading = 0;

    printf("req: %s \n", clientRequest);


    // char buff[1024] = {0};
    // while((client_bytes_read = recv(client_socket, buff, 1024, 0)))
    // {
    //     // if(client_bytes_read == 0)
    //     //     break;
    //     // printf("---> %zu\n", client_bytes_read);
    //     req.append(buff);
    //     // if((clientRequest[clientmsgsize-1] == '\n' && clientRequest[clientmsgsize-2] == '\r' && clientRequest[clientmsgsize-3] == '\n' && clientRequest[clientmsgsize-4] == '\r'))
    //     //     break;
    // }

    
    return (req);
}

void handle_client_request(int client_socket)
{
    std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello w!\r\n\r\n";

    // send(client_socket, "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello w!", hello.size(), 0);
    // write(client_socket, "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello w!\r\n\r\n", hello.size());


    int msgsentsize = 0;
    ssize_t bytes_sent = 0;
    while((bytes_sent = send(client_socket, &hello[msgsentsize], hello.size() - msgsentsize, 0)))
    {
        // printf("---+-+-+-+-+-+-+-+-+-+----> %d\n", bytes_sent);
        msgsentsize += bytes_sent;
        if(msgsentsize == hello.size())
        {
            break;
        }
    }
    if(bytes_sent == -1)
        printf("send : an error occured \n");
}

int main(int argc, char const *argv[])
{

    int server_fd;
    server_fd = set_server();

    // pollfd pfd[2];
    pollfd poll_fd;
    nfds_t poll_len = 0;



    // pollFd = malloc(1 * sizeof(struct pollfd));
    // if(pollFd == NULL){
    //     perror("Error malloc");
    //     exit(EXIT_FAILURE);
    // }
    // poll_len = 1;

    poll_fd.fd = server_fd;
    poll_fd.events = POLLIN; // watch for recv

    poll_fds.push_back(poll_fd);

    printf("Server_fd = %d\n\n\n\n", server_fd);

    int got_req = 0;
    while(1)
    {
        // printf("Waiting .... \n");
        int poll_res = poll(&poll_fds.front(), poll_fds.size(), -1);
        // printf("poll_res = %d \n", poll_res);
        if (poll_res < 0)
        {
            perror("Error poll");
            exit(EXIT_FAILURE);
        }
        else if (poll_res == 0)
        {
            printf("Poll is waiting for new events");
            continue;
        }
       
        int poll_size = poll_fds.size();
        for (int i = 0; i < poll_size; i++)
        {
            if(poll_fds[i].revents == 0)
                continue;



            
            if (poll_fds[i].revents & POLLIN) // Ready to read
            {
                if(poll_fds[i].fd == server_fd)
                {
                    int client_socket = new_client(server_fd);
                    poll_fd.fd = client_socket;
                    poll_fd.events =  POLLIN | POLLOUT;
                    poll_fd.revents = 0;
                    poll_fds.push_back(poll_fd);
                    Client_socket cs;
                    cs.fd = client_socket;
                    cs.reading = 0;
                    cs.writing = 0;
                    clients.insert(std::pair<int, Client_socket>(client_socket, cs));
                    printf("New client = +1, fd = %d, revents : %d\n",client_socket, poll_fds[i].revents);
                }
                else
                {
                    printf("POLLIN =====> fd : %d, revents : %d\n", poll_fds[i].fd, poll_fds[i].revents);
                    got_req = 1;
                    // exit(1);
                    std::string req = client_request(poll_fds[i].fd);
                }
            }
            if ((poll_fds[i].revents & POLLOUT) && got_req) // Ready to write
            {
                printf("POLLOUT =====> fd : %d, revents : %d\n", poll_fds[i].fd, poll_fds[i].revents);
                handle_client_request(poll_fds[i].fd);
                got_req = 0;
                // close(poll_fds[i].fd);
                // poll_fds.erase(poll_fds.begin() + i);

            }
            if (poll_fds[i].revents & POLLHUP)
            {
                printf("POLLHUP =====> fd : %d, revents : %d\n", poll_fds[i].fd, poll_fds[i].revents);
                close(poll_fds[i].fd);
                poll_fds.erase(poll_fds.begin() + i);
            }
            if (poll_fds[i].revents & POLLNVAL)
            {
                printf("POLLNVAL =====> fd : %d, revents : %d\n", poll_fds[i].fd, poll_fds[i].revents);
                poll_fds.erase(poll_fds.begin() + i);
            }


            // printf("-----------------------------------\n");
         
        }
        printf("\n\n\n\n\n\n");
    }
    return 0;
}
