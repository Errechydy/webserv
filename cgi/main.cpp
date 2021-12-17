#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
extern char **environ;

int main(int argc, char **argvg, char **env)
{


    std::string cgi_location = "/goinfre/ler-rech/.brew/bin/php-cgi"; 
    std::string req_file = "/Users/ler-rech/Desktop/webserv/cgi/php.php";
    char * const args[3] = {const_cast<char *>(cgi_location.c_str()), const_cast<char *>(req_file.c_str()), NULL};
    
    std::vector<const char *> vec;
    vec.push_back(strdup((std::string("REDIRECT_STATUS") + "=200").c_str()));
    vec.push_back(strdup((std::string("GATEWAY_INTERFACE") + "=CGI/1.1").c_str()));
    vec.push_back(strdup((std::string("SERVER_NAME") + "= MY_SERVER_NAME").c_str()));
    vec.push_back(strdup((std::string("SERVER_SOFTWARE") + "=Webserv 1.0").c_str()));
    vec.push_back(strdup((std::string("PATH_TRANSLATED") + "=/Users/ler-rech/Desktop/webserv/cgi/php.php").c_str()));
    vec.push_back(strdup((std::string("REQUEST_METHOD") + "=GET").c_str()));
    vec.push_back(strdup((std::string("PATH_INFO") + "= ?a=value1&b=value2").c_str()));
    vec.push_back(strdup((std::string("SERVER_PROTOCOL") + "=HTTP/1.1").c_str()));
    vec.push_back(strdup((std::string("SERVER_PORT") + "=80").c_str()));
    vec.push_back(strdup((std::string("QUERY_STRING") + "=a=value1&b=value2").c_str()));
    vec.push_back(strdup((std::string("SCRIPT_NAME") + "=/Users/ler-rech/Desktop/webserv/cgi").c_str()));
    vec.push_back(strdup((std::string("REMOTE_HOST") + "=HostName.com").c_str()));
    vec.push_back(strdup((std::string("REMOTE_ADDR") + "=875.65.158.33").c_str()));
    vec.push_back(strdup((std::string("CONTENT_TYPE") + "=text/html").c_str())); // If the request includes a message-body, the CONTENT_TYPE variable is set to the Internet Media Type [6] of the message-body.
    // vec.push_back(strdup((std::string("CONTENT_LENGTH") + "=900").c_str()));
    vec.push_back(NULL);



    if (execve(args[0], args, const_cast<char * const *>(&vec[0])) == -1)
        perror("Could not execve fff");

    return (0);
}



