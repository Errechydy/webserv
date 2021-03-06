#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
 #include <fcntl.h>

std::string to_string(int n)
{
    std::ostringstream string;
    string << n;
    return string.str();
}

int main(int argc, char **argvg, char **env)
{

/*
    std::string cgi_location = "./cgi";
    std::string req_file = "/var/ls/fsdfs.php";
    char * const args[3] = {const_cast<char *>(cgi_location.c_str()), const_cast<char *>(req_file.c_str()), NULL};

    std::vector<const char *> vec;
    std::string method = "GET";
    std::string server_name = "SERVER_NAME"; // The server's hostname or IP address.
    std::string server_software = "Webserv 1.0"; // The name and version of the server software that is answering the client request.
    int port = 8080;
    std::string file_path = "sfdsf/sdfds/dsfds.fds"; // requested file_path
    std::string path_info = "sfdsf/sdfds/dsfds"; // url until the first "?" 
    std::string query_path = "?sfdsf/sdfds/dsfds"; // url from the first "?"  to the end
    std::string document_root = "/var/sfdsf/sdfds/home/"; // The directory from which Web documents are served.
    std::string script_name = "/var/file.php"; // The path to the executed file
    std::string remote_host = "/var/file.php"; // The remote hostname of the user making the request, from where the request is made req.get('Host'), example : www.google.com
    std::string remote_address = "875.65.158.33"; // The remote IP address of the user making the request.
    std::string content_type = "text/html"; // The request content type req.get("Content-Type")
    int content_length = 520; // The length of the data (in bytes) req.get("Content-Length")
    std::string accepted_types = "text/html"; // A list of the MIME types that the client can accept. req.get("Accept")
    std::string user_agent = "Mozilla/5.0 ..."; // User agent. req.get("User-Agent")
    std::string referer = " https://www.cplusplus.com/reference/vector/vector/?kw=vector...."; // The URL of the document that the client points to before accessing the CGI program. req.get("Referer")
    

    vec.push_back(strdup((std::string("GATEWAY_INTERFACE") + "=CGI/1.1").c_str()));
    vec.push_back(strdup((std::string("SERVER_NAME") + "=" + server_name).c_str()));
    vec.push_back(strdup((std::string("SERVER_SOFTWARE") + "=" + server_software).c_str()));
    vec.push_back(strdup((std::string("SERVER_PROTOCOL") + "=HTTP/1.1").c_str()));
    vec.push_back(strdup((std::string("SERVER_PORT") + "=" + to_string(port)).c_str()));
    vec.push_back(strdup((std::string("REQUEST_METHOD") + "=" + method).c_str()));
    vec.push_back(strdup((std::string("PATH_INFO") + "=" + path_info).c_str()));
    vec.push_back(strdup((std::string("PATH_TRANSLATED") + "=" + file_path).c_str()));
    vec.push_back(strdup((std::string("QUERY_STRING") + "=" + query_path).c_str()));
    vec.push_back(strdup((std::string("DOCUMENT_ROOT") + "=" + document_root).c_str()));
    vec.push_back(strdup((std::string("SCRIPT_NAME") + "=" + script_name).c_str()));
    vec.push_back(strdup((std::string("REMOTE_HOST") + "=" + remote_host).c_str()));
    vec.push_back(strdup((std::string("REMOTE_ADDR") + "=" + remote_address).c_str()));
    vec.push_back(strdup((std::string("CONTENT_TYPE") + "=" + content_type).c_str()));
    vec.push_back(strdup((std::string("CONTENT_LENGTH") + "=" + to_string(content_length)).c_str()));
    vec.push_back(strdup((std::string("HTTP_ACCEPT") + "=" + accepted_types).c_str()));
    vec.push_back(strdup((std::string("HTTP_USER_AGENT") + "=" + user_agent).c_str()));
    vec.push_back(strdup((std::string("HTTP_REFERER") + "=" + referer).c_str()));
    vec.push_back(NULL);

    if (execve(args[0], args, const_cast<char * const *>(&vec[0])) == -1)
        perror("Could not execve fff");
*/


/*

    int pipe_fd[2];
    // pipe_fd[0] - read
    // pipe_fd[1] - wrtie

    if(pipe(pipe_fd) == -1)
    {
        std::cout << "An error ocurred with opening the pipe" << std::endl;
        return (1);
    }

    int fork_id = fork();

    if(fork_id == 0) // child process
    {
        int x = 64686887;
        close(pipe_fd[0]); // close the read end
        write(pipe_fd[1], &x, sizeof(int));
        close(pipe_fd[1]); // close the write end after finishing writing
    }
    else // parent process
    {
        int y;
        close(pipe_fd[1]); // close the write end because we don't need it
        read(pipe_fd[0], &y, sizeof(int));
        close(pipe_fd[0]); // close the read end after finishing reading
        std::cout << "Got the data in the parent process : " << y << std::endl;
    }
*/    








    int pipe_fd[2];
    // pipe_fd[0] - read
    // pipe_fd[1] - wrtie
    // file descriptor 0 : stdin
    // file descriptor 1 : stdout
    // file descriptor 2 : stderr

    if(pipe(pipe_fd) == -1)
    {
        std::cout << "An error ocurred with opening the pipe" << std::endl;
        return (1);
    }

    int fork_id = fork();
    if(fork_id == -1) // child process
    {
        std::cout << "An error ocurred when forking" << std::endl;
        return (1);
    }
    else if(fork_id == 0) // child process
    {
        close(pipe_fd[0]); // close the read end
        dup2(pipe_fd[1], 1);

        std::string cgi_location = "./cgi";
        std::string req_file = "/var/ls/fsdfs.php";
        char * const args[3] = {const_cast<char *>(cgi_location.c_str()), const_cast<char *>(req_file.c_str()), NULL};

        std::vector<const char *> vec;
        std::string method = "GET";
        std::string server_name = "SERVER_NAME"; // The server's hostname or IP address.
        std::string server_software = "Webserv 1.0"; // The name and version of the server software that is answering the client request.
        int port = 8080;
        std::string file_path = "sfdsf/sdfds/dsfds.fds"; // requested file_path
        std::string path_info = "sfdsf/sdfds/dsfds"; // url until the first "?" 
        std::string query_path = "?sfdsf/sdfds/dsfds"; // url from the first "?"  to the end
        std::string document_root = "/var/sfdsf/sdfds/home/"; // The directory from which Web documents are served.
        std::string script_name = "/var/file.php"; // The path to the executed file
        std::string remote_host = "/var/file.php"; // The remote hostname of the user making the request, from where the request is made req.get('Host'), example : www.google.com
        std::string remote_address = "875.65.158.33"; // The remote IP address of the user making the request.
        std::string content_type = "text/html"; // The request content type req.get("Content-Type")
        int content_length = 520; // The length of the data (in bytes) req.get("Content-Length")
        std::string accepted_types = "text/html"; // A list of the MIME types that the client can accept. req.get("Accept")
        std::string user_agent = "Mozilla/5.0 ..."; // User agent. req.get("User-Agent")
        std::string referer = " https://www.cplusplus.com/reference/vector/vector/?kw=vector...."; // The URL of the document that the client points to before accessing the CGI program. req.get("Referer")
        

        vec.push_back(strdup((std::string("GATEWAY_INTERFACE") + "=CGI/1.1").c_str()));
        vec.push_back(strdup((std::string("SERVER_NAME") + "=" + server_name).c_str()));
        vec.push_back(strdup((std::string("SERVER_SOFTWARE") + "=" + server_software).c_str()));
        vec.push_back(strdup((std::string("SERVER_PROTOCOL") + "=HTTP/1.1").c_str()));
        vec.push_back(strdup((std::string("SERVER_PORT") + "=" + to_string(port)).c_str()));
        vec.push_back(strdup((std::string("REQUEST_METHOD") + "=" + method).c_str()));
        vec.push_back(strdup((std::string("PATH_INFO") + "=" + path_info).c_str()));
        vec.push_back(strdup((std::string("PATH_TRANSLATED") + "=" + file_path).c_str()));
        vec.push_back(strdup((std::string("QUERY_STRING") + "=" + query_path).c_str()));
        vec.push_back(strdup((std::string("DOCUMENT_ROOT") + "=" + document_root).c_str()));
        vec.push_back(strdup((std::string("SCRIPT_NAME") + "=" + script_name).c_str()));
        vec.push_back(strdup((std::string("REMOTE_HOST") + "=" + remote_host).c_str()));
        vec.push_back(strdup((std::string("REMOTE_ADDR") + "=" + remote_address).c_str()));
        vec.push_back(strdup((std::string("CONTENT_TYPE") + "=" + content_type).c_str()));
        vec.push_back(strdup((std::string("CONTENT_LENGTH") + "=" + to_string(content_length)).c_str()));
        vec.push_back(strdup((std::string("HTTP_ACCEPT") + "=" + accepted_types).c_str()));
        vec.push_back(strdup((std::string("HTTP_USER_AGENT") + "=" + user_agent).c_str()));
        vec.push_back(strdup((std::string("HTTP_REFERER") + "=" + referer).c_str()));
        vec.push_back(NULL);

        if (execve(args[0], args, const_cast<char * const *>(&vec[0])) == -1)
            perror("Could not execve fff");

        close(pipe_fd[1]); // close the write end after finishing writing
    }
    else // parent process
    {
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


        close(pipe_fd[1]); // close the write end because we don't need it
        int nbytes;
        char cgi_buff[1024] = {0};

        // Read the data from pipe_fd[0], and search for EOF or content_length
        while ((nbytes = read(pipe_fd[0], cgi_buff, 1024)) > 0) { 
                std::cout << "Got some data from pipe : " << cgi_buff << std::endl;
        }
        close(pipe_fd[0]); // close the read end after finishing reading
    }

    return (0);
}



