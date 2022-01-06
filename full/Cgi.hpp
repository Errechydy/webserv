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
        std::string					req_file;
        std::string					cookies;
        std::string					uri;
        std::string					req_method;
        std::string					req_content_type;
        std::string					req_content_length;
        std::vector<const char *>	cgi_args;
	    int							pipe_fd[2];
        int							body_pipe_fd[2];


	public:
		explicit Cgi_class(Location loc, std::string req_f, std::string cookies_v, std::string uri_v, std::string req_meth, std::string req_cont_ty, std::string req_cont_len)
        : location(loc), req_file(req_f), cookies(cookies_v), uri(uri_v), req_method(req_meth), req_content_type(req_cont_ty), req_content_length(req_cont_len) {
            int exec_return;
            char * const args[3] = {const_cast<char *>(location.cgi_path.c_str()), const_cast<char *>(req_file.c_str()), NULL};
            exec_return = execute_cgi(args);
        }
		~Cgi_class();
        int execute_cgi(char * const args[]);
		int send_cgi_body(std::string cgi_body);
		void set_cgi_args();
        std::string to_string(int n);
};

#endif