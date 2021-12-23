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




int main(int argc, char const *argv[])
{

    std::string uri = "/1.php?userId=54545&b=value2";
	std::string params = uri.substr(uri.find_first_of("?")+1);
	std::cout << "Params : " << params << std::endl;
    return (0);
}