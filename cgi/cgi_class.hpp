#ifndef CGI_CLASS_HPP
# define CGI_CLASS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>


struct Location {
	int                 port;
    std::string         host;
    std::string         server_name;
    std::string         error_page;             // exp: error.html
    std::string         client_max_body_size;   // in kb,  "" unlimited,
    std::string         method;                 // GET, POST, DELETE
    std::string         redirect;               // /abc/ef/
    std::string         root;                   // /www/html/
    std::string         autoindex;             // 0, 1
    std::string         default_answer;         // index.html
    std::string         cgi_extension;          // php
    std::string         cgi_path;               // localhost:9000
    std::string         accept_upload;          // 0, 1
    std::string         upload_path;            // /www/html/upload
};



class Cgi_class
{

	public:
        Location					location;
        std::string					req_file;
        std::string					cookies;
        std::vector<const char *>	cgi_args;
	    int							pipe_fd[2];
        int							body_pipe_fd[2];


	public:
		explicit Cgi_class(Location loc, std::string req_f, std::string cookies_v) : location(loc), req_file(req_f) , cookies(cookies_v) {
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