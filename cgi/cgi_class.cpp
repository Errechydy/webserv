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
        std::string					req_file; // TODO: this will be request object so i can fill the cgi_args that are related to the client
        std::vector<const char *>	cgi_args;
	    int							pipe_fd[2];

	public:
		explicit Cgi_class(Location loc, std::string req_f) : location(loc), req_file(req_f) {
            int exec_return;
            char * const args[3] = {const_cast<char *>(location.cgi_path.c_str()), const_cast<char *>(req_file.c_str()), NULL};
            exec_return = execute_cgi(args);
            if(!exec_return)
                exit(1);
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
				close(pipe_fd[1]);
				set_cgi_args();
				if (execve(args[0], args, const_cast<char * const *>(&cgi_args[0])) == -1)
					perror("Could not execve fff");
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
		
				close(pipe_fd[1]);
				int nbytes;
				char cgi_buff[1024] = {0};
				// Read the data from pipe_fd[0], and search for EOF or content_length
				while ((nbytes = read(pipe_fd[0], cgi_buff, 1024)) > 0) { 
						std::cout << "Got some data from pipe : " << cgi_buff << std::endl;
				}
				close(pipe_fd[0]); // close the read end after finishing reading, don't close it until you finish reading all cgi response (EOF or read content_length if the response has content_length)
			}
        }

		void set_cgi_args()
        {
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
			


			cgi_args.push_back(strdup((std::string("GATEWAY_INTERFACE") + "=CGI/1.1").c_str()));
			cgi_args.push_back(strdup((std::string("SERVER_NAME") + "=" + server_name).c_str()));
			cgi_args.push_back(strdup((std::string("SERVER_SOFTWARE") + "=" + server_software).c_str()));
			cgi_args.push_back(strdup((std::string("SERVER_PROTOCOL") + "=HTTP/1.1").c_str()));
			cgi_args.push_back(strdup((std::string("SERVER_PORT") + "=" + to_string(port)).c_str()));
			cgi_args.push_back(strdup((std::string("REQUEST_METHOD") + "=" + method).c_str()));
			cgi_args.push_back(strdup((std::string("PATH_INFO") + "=" + path_info).c_str()));
			cgi_args.push_back(strdup((std::string("PATH_TRANSLATED") + "=" + file_path).c_str()));
			cgi_args.push_back(strdup((std::string("QUERY_STRING") + "=" + query_path).c_str()));
			cgi_args.push_back(strdup((std::string("DOCUMENT_ROOT") + "=" + document_root).c_str()));
			cgi_args.push_back(strdup((std::string("SCRIPT_NAME") + "=" + script_name).c_str()));
			cgi_args.push_back(strdup((std::string("REMOTE_HOST") + "=" + remote_host).c_str()));
			cgi_args.push_back(strdup((std::string("REMOTE_ADDR") + "=" + remote_address).c_str()));
			cgi_args.push_back(strdup((std::string("CONTENT_TYPE") + "=" + content_type).c_str()));
			cgi_args.push_back(strdup((std::string("CONTENT_LENGTH") + "=" + to_string(content_length)).c_str()));
			cgi_args.push_back(strdup((std::string("HTTP_ACCEPT") + "=" + accepted_types).c_str()));
			cgi_args.push_back(strdup((std::string("HTTP_USER_AGENT") + "=" + user_agent).c_str()));
			cgi_args.push_back(strdup((std::string("HTTP_REFERER") + "=" + referer).c_str()));
			cgi_args.push_back(NULL);
        }

        std::string to_string(int n)
        {
            std::ostringstream string;
            string << n;
            return string.str();
        }
};