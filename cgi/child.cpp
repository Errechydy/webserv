#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
extern char **environ;

std::string to_string(int n)
{
    std::ostringstream string;
    string << n;
    return string.str();
}

int main(int argc, char **argvg, char **env)
{
    int pipe_fd[2];
int body_pipe_fd[2];

    if(pipe(pipe_fd) == -1)
    {
        std::cout << "An error ocurred with opening the pipe" << std::endl;
        return (1);
    }
    if(pipe(body_pipe_fd) == -1)
    {
        std::cout << "An error ocurred with opening the pipe" << std::endl;
        return (1);
    }


    int fork_id = fork();
    if(fork_id == -1)
    {
        std::cout << "An error ocurred when forking" << std::endl;
        return (1);
    }
    else if(fork_id == 0) // child process
    {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], 1);
        close(pipe_fd[1]); // close the write end after finishing writing


        
        dup2(body_pipe_fd[0], 0);
        close(body_pipe_fd[0]);
        close(body_pipe_fd[1]);



        std::string cgi_location = "/goinfre/ler-rech/.brew/bin/php-cgi"; 
        std::string req_file = "/Users/ler-rech/Desktop/webserv/cgi/php.php";
        char * const args[3] = {const_cast<char *>(cgi_location.c_str()), const_cast<char *>(req_file.c_str()), NULL};
        
        std::vector<const char *> vec;
        vec.push_back(strdup((std::string("REDIRECT_STATUS") + "=200").c_str()));
        vec.push_back(strdup((std::string("GATEWAY_INTERFACE") + "=CGI/1.1").c_str()));
        vec.push_back(strdup((std::string("SERVER_NAME") + "= MY_SERVER_NAME").c_str()));
        vec.push_back(strdup((std::string("SERVER_SOFTWARE") + "=Webserv 1.0").c_str()));
        vec.push_back(strdup((std::string("PATH_TRANSLATED") + "=/Users/ler-rech/Desktop/webserv/cgi/php.php").c_str()));
        vec.push_back(strdup((std::string("REQUEST_METHOD") + "=POST").c_str()));
        vec.push_back(strdup((std::string("PATH_INFO") + "= ?userId=54545&b=value2").c_str()));
        vec.push_back(strdup((std::string("SERVER_PROTOCOL") + "=HTTP/1.1").c_str()));
        vec.push_back(strdup((std::string("SERVER_PORT") + "=80").c_str()));
        vec.push_back(strdup((std::string("QUERY_STRING") + "=userId=54545&b=value2").c_str())); // ....com/sfds?a=sdjkfhdks
        vec.push_back(strdup((std::string("SCRIPT_NAME") + "=/Users/ler-rech/Desktop/webserv/cgi").c_str()));
        vec.push_back(strdup((std::string("REMOTE_HOST") + "=HostName.com").c_str()));
        vec.push_back(strdup((std::string("REMOTE_ADDR") + "=875.65.158.33").c_str()));
        // vec.push_back(strdup((std::string("CONTENT_TYPE") + "=application/x-www-form-urlencoded").c_str())); // If the request includes a message-body, the CONTENT_TYPE variable is set to the Internet Media Type [6] of the message-body.
        vec.push_back(strdup((std::string("CONTENT_TYPE") + "=application/x-www-form-urlencoded").c_str())); // If the request includes a message-body, the CONTENT_TYPE variable is set to the Internet Media Type [6] of the message-body.
        // vec.push_back(strdup((std::string("CONTENT_LENGTH") + "=NULL").c_str())); // or 5454 in bytes
        vec.push_back(strdup((std::string("CONTENT_LENGTH") + "=10").c_str()));
        vec.push_back(NULL);


        if (execve(args[0], args, const_cast<char * const *>(&vec[0])) == -1)
            perror("Could not execve fff");

    }
    else // parent process
    {

        close(pipe_fd[1]);
        close(body_pipe_fd[0]);

        std::string message = "v=1234567";
        write(body_pipe_fd[1], message.c_str(), message.size() + 1);



        int wstatus;
        wait(&wstatus);
        if (WIFEXITED(wstatus))
        {
            int status_code = WEXITSTATUS(wstatus);
            if(status_code != 0)
            {
                std::cout << "Failure with status code : " << status_code << std::endl;
                return (1);
            }
        }

        std::string resp;
  

        int nbytes;
        char cgi_buff[1024] = {0};
        // Read the data from pipe_fd[0], and search for EOF or content_length
        while ((nbytes = read(pipe_fd[0], cgi_buff, 1024)) > 0) { 
                // std::cout << "Got some data from pipe : " << cgi_buff << std::endl;
                resp+= cgi_buff;
                // std::cout << cgi_buff << std::endl;
        }

                std::cout << resp << std::endl;

        close(pipe_fd[0]); // close the read end after finishing reading, don't close it until you finish reading all cgi response (EOF or read content_length if the response has content_length)
    }

    return (0);
}



