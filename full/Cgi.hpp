#ifndef CGI_CLASS_HPP
# define CGI_CLASS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "ConfigFile.hpp"

class Cgi_class
{

	public:
        Location					location;
        std::string					cookies;
        std::string					req_file;
        std::vector<const char *>	cgi_args;
	    int							pipe_fd[2];
        int							body_pipe_fd[2];


	public:
		explicit Cgi_class(Location loc, std::string req_f, std::string cookies_v) : location(loc) , cookies(cookies_v) {
            req_file = location.root+req_f;
            int exec_return;
            char * const args[3] = {const_cast<char *>(location.cgi_path.c_str()), const_cast<char *>(req_file.c_str()), NULL};
            exec_return = execute_cgi(args);
            // if(!exec_return)
            //     exit(1);
        }
		~Cgi_class();
        int execute_cgi(char * const args[]);
		int send_cgi_body(std::string cgi_body);
		void set_cgi_args();
        std::string to_string(int n);
};

#endif