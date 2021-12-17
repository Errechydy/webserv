#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>


struct Location {
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
        std::vector<const char *>	cgi_args;
	    int							pipe_fd[2];
        int							body_pipe_fd[2];


	public:
		explicit Cgi_class(Location loc, std::string req_f) : location(loc), req_file(req_f) {
            int exec_return;
            char * const args[3] = {const_cast<char *>(location.cgi_path.c_str()), const_cast<char *>(req_file.c_str()), NULL};
            exec_return = execute_cgi(args);
            // if(!exec_return)
            //     exit(1);
        }
		~Cgi_class() {
        }

        int execute_cgi(char * const args[])
        {
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
				dup2(pipe_fd[1], 1); // 1: STDOUT
				close(pipe_fd[0]);
				close(pipe_fd[1]);
				set_cgi_args();

				dup2(body_pipe_fd[0], 0); // 0 : stdin
				close(body_pipe_fd[1]);
				close(body_pipe_fd[0]);

				if (execve(args[0], args, const_cast<char * const *>(&cgi_args[0])) == -1)
					perror("Could not execve fff");
			}
			else // parent process
			{
				close(pipe_fd[1]);
				close(body_pipe_fd[0]);
			
			}
			return (0);
        }

		int send_cgi_body(std::string cgi_body)
        {
    			write(body_pipe_fd[1], cgi_body.c_str(), cgi_body.size() + 1);

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
				return (0);
		}

		void set_cgi_args()
        {
			cgi_args.push_back(strdup((std::string("REDIRECT_STATUS") + "=200").c_str()));
			cgi_args.push_back(strdup((std::string("GATEWAY_INTERFACE") + "=CGI/1.1").c_str()));
			cgi_args.push_back(strdup((std::string("SERVER_NAME") + "= MY_SERVER_NAME").c_str()));
			cgi_args.push_back(strdup((std::string("SERVER_SOFTWARE") + "=Webserv 1.0").c_str()));
			cgi_args.push_back(strdup((std::string("PATH_TRANSLATED") + "="+req_file).c_str()));
			cgi_args.push_back(strdup((std::string("PATH_INFO") + "= ?userId=54545&b=value2").c_str()));
			cgi_args.push_back(strdup((std::string("SERVER_PROTOCOL") + "=HTTP/1.1").c_str()));
			cgi_args.push_back(strdup((std::string("SERVER_PORT") + "=80").c_str()));
			cgi_args.push_back(strdup((std::string("QUERY_STRING") + "=userId=54545&b=value2").c_str())); // ....com/sfds?a=sdjkfhdks
			cgi_args.push_back(strdup((std::string("REMOTE_HOST") + "=HostName.com").c_str()));
			cgi_args.push_back(strdup((std::string("REMOTE_ADDR") + "=875.65.158.33").c_str()));


			// cgi_args.push_back(strdup((std::string("REQUEST_METHOD") + "=GET").c_str()));
			// cgi_args.push_back(strdup((std::string("CONTENT_TYPE") + "=text/html").c_str())); // If the request includes a message-body, the CONTENT_TYPE variable is set to the Internet Media Type [6] of the message-body.
			// cgi_args.push_back(strdup((std::string("CONTENT_LENGTH") + "=NULL").c_str())); 

			
			cgi_args.push_back(strdup((std::string("REQUEST_METHOD") + "=POST").c_str()));
			cgi_args.push_back(strdup((std::string("CONTENT_TYPE") + "=application/x-www-form-urlencoded").c_str())); // If the request includes a message-body, the CONTENT_TYPE variable is set to the Internet Media Type [6] of the message-body.
			cgi_args.push_back(strdup((std::string("CONTENT_LENGTH") + "=10").c_str())); // // or 10 bytes


			cgi_args.push_back(NULL);


        }

        std::string to_string(int n)
        {
            std::ostringstream string;
            string << n;
            return string.str();
        }
};