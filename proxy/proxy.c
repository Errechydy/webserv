// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8081


// char* cleanClientRequest(char clientRequest[300000], int len)
// {
//     //    printf("Client ======> %s\n", *clientRequest[300000]);
//     int i = 0;
//     int j = 0;
//     char clientCleanRequest[300000] = {0};
//     int skip = 0;

//     // while(i < len)
//     // {
//     //     if(clientRequest[i] == 'H' && clientRequest[i+1] == 'o' && clientRequest[i+2] == 's' && clientRequest[i+3] == 't')
//     //     {
//     //         i += 22;
//     //     }
//     //     else if(clientRequest[i] == 'R' && clientRequest[i+1] == 'e' && clientRequest[i+2] == 'f' && clientRequest[i+3] == 'e')
//     //         i += 33;


//     //     clientCleanRequest[j] = clientRequest[i];
//     //     j++;
//     //     i++;
//     // }

//     // i = 0;
//     // while(i <= j)
//     // {
//     //     clientRequest[i] = clientCleanRequest[i];
//     //     i++;
//     // }



//     while(i < len)
//     {
//         if(clientRequest[i] == 'H' && clientRequest[i+1] == 'o' && clientRequest[i+2] == 's' && clientRequest[i+3] == 't')
//             break;
//         i++;
//     }

//     char* req;

//     req = malloc(sizeof(char) * (i + 26));

//     j = 0;
//     while(j < i)
//     {
//         req[j] = clientRequest[j];
//         j++;
//     }
    
//     char* f = "Connection: keep-alive\r\n\r\n";
//     i += 26;
//     int k = 0;
//     while(j < i)
//     {
//         req[j] = f[k];
//         k++;
//         j++;
//     }





//     // printf("rrrrr = %s \n", req);


//     return (req);
// }





char* cleanClientRequest(char clientRequest[300000], int len)
{
    //    printf("Client ======> %s\n", *clientRequest[300000]);
    int i = 0;
    int j = 0;
    char clientCleanRequest[300000] = {0};
    int skip = 0;

    while(i < len)
    {
        if(clientRequest[i] == 'H' && clientRequest[i+1] == 'o' && clientRequest[i+2] == 's' && clientRequest[i+3] == 't')
        {
            i += 22;
        }
        else if(clientRequest[i] == 'R' && clientRequest[i+1] == 'e' && clientRequest[i+2] == 'f' && clientRequest[i+3] == 'e')
            i += 33;


        clientCleanRequest[j] = clientRequest[i];
        j++;
        i++;
    }
    clientCleanRequest[j] = '\0';

    char* r = malloc(sizeof(char) * (j + 1));

    i = 0;
    while(i <= j)
    {
        r[i] = clientCleanRequest[i];
        i++;
    }
    r[i] = '\0';
    // int rdddd = strlen(r);


    // printf("-------> %d \n", r[rdddd]);
    // printf("-------> %d \n", r[rdddd-1]);
    // printf("-------> %d \n", r[rdddd-2]);
    // printf("-------> %d \n", r[rdddd-3]);
    // printf("-------> %d \n", r[rdddd-4]);
    // printf("-------> %d \n\n\n\n", r[rdddd-5]);

    return (r);

}



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


        /***************************************************************************/
        /**************************** Read client request **************************/
        /***************************************************************************/
        char clientRequest[300000] = {0};
        int clientmsgsize = 0;
        size_t client_bytes_read;
        while((client_bytes_read = recv(new_socket, clientRequest + clientmsgsize, sizeof(clientRequest) - clientmsgsize - 1, 0)))
        {
            // printf("---> %zu\n", client_bytes_read);
            clientmsgsize += client_bytes_read;
            if(clientmsgsize > 300000 - 1 || (clientRequest[clientmsgsize-1] == '\n' && clientRequest[clientmsgsize-2] == '\r' && clientRequest[clientmsgsize-3] == '\n' && clientRequest[clientmsgsize-4] == '\r'))
                break;
        }


    //    printf("Client ======> %s\n",clientRequest);
    //    cleanClientRequest(clientRequest, clientmsgsize);
    //    printf("%s\n",clientRequest);






        /***************************************************************************/
        /************ Creat socket to connect to external server *******************/
        /***************************************************************************/

        if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("In socket");
            exit(EXIT_FAILURE);
        }



        /***************************************************************************/
        /************************* Request external server *************************/
        /***************************************************************************/

        char* IP = "142.250.200.78"; // google
        // char* IP = "13.58.131.53";
        address2.sin_family = AF_INET;
        address2.sin_port = htons( 80 );

        //char* req = "GET /xjs/_/js/k=xjs.hp.en.U0uNpJAarw4.O/am=ACcAVg/d=1/ed=1/esmo=1/rs=ACT90oEt2sr6IAdENmU0z26CrgCaVUp1XA/m=sb_he,d HTTP/1.1\r\nConnection: keep-alive\r\nAccept: text/html,application/json\r\n\r\n";
        
        char* req = cleanClientRequest(clientRequest, clientmsgsize);

        //printf("===> %s \n", req2);


        // int r = strlen(req);

        //     printf("rr-------> %d \n", req[r]);
        //     printf("-------> %d \n", req[r-1]);
        //     printf("-------> %d \n", req[r-2]);
        //     printf("-------> %d \n", req[r-3]);
        //     printf("-------> %d \n", req[r-4]);
        //     printf("-------> %d \n\n\n\n", req[r-5]);


        
        if(inet_pton(AF_INET, IP, &address2.sin_addr) <= 0)
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
        // send(client_fd , clientRequest , strlen(clientRequest), 0);


        /***************************************************************************/
        /************************** Read external server reaponse ******************/
        /***************************************************************************/
        char buffer[300000] = {0};
        int msgsize = 0;
        size_t bytes_read;
        // fcntl(client_fd, F_SETFL, O_NONBLOCK);
        while((bytes_read = recv(client_fd, buffer + msgsize, sizeof(buffer) - msgsize - 1, 0)))
        {
            // printf("--++++++++-> %zu\n", bytes_read);
            msgsize += bytes_read;
            if(msgsize > 300000 - 1 || (buffer[msgsize-1] == '\n' && buffer[msgsize-2] == '\r' && buffer[msgsize-3] == '\n' && buffer[msgsize-4] == '\r'))
                break;
        }
    //    printf("======> %s\n",buffer);




        /***************************************************************************/
        /************** Send external server reaponse to the client ****************/
        /***************************************************************************/
        int msgsentsize = 0;
        int bytes_sent = 0;
        while((bytes_sent = send(new_socket, buffer + msgsentsize, sizeof(buffer) - msgsentsize - 10, 0)))
        {
            // printf("---+-+-+-+-+-+-+-+-+-+----> %zu\n", bytes_read);
            msgsentsize += bytes_sent;
            if(msgsentsize == sizeof(buffer))
                break;
        }
        printf("------------------ Request sent-------------------");
        //close(new_socket);
    }
    close(server_fd);
    return 0;
}