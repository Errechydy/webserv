

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

int main()
{
    char tab[10] = "a";
    // tab[1] = 'a';
    // tab[2] = 'b';
    // tab[3] = 'c';
    // tab[4] = 'd';
    // tab[5] = '\n';
    // tab[6] = '\0';

    std::string s;
    s.reserve(6);
    // s[0] = 'a';
    // s[1] = '\0';

    s.append(tab);

    std::cout << "s : " << s << std::endl;
    std::cout << "size : " << s.size() << std::endl;
    std::cout << "cap : " << s.capacity() << std::endl;
   
    // printf("char = %c \n", tab[0]);

    return (0);
}