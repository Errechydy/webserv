# include "Cgi.hpp"



Cgi_class::~Cgi_class() {
}

int Cgi_class::execute_cgi(char * const args[])
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
			perror("Could not execve");
	}
	else // parent process
	{
		close(pipe_fd[1]);
		close(body_pipe_fd[0]);
	
	}
	return (0);
}

int Cgi_class::send_cgi_body(std::string cgi_body)
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

void Cgi_class::set_cgi_args()
{
	std::string params = uri.substr(uri.find_first_of("?")+1);
	cgi_args.push_back(strdup((std::string("REDIRECT_STATUS") + "=200").c_str()));
	cgi_args.push_back(strdup((std::string("GATEWAY_INTERFACE") + "=CGI/1.1").c_str()));
	cgi_args.push_back(strdup((std::string("SERVER_PROTOCOL") + "=HTTP/1.1").c_str()));
	cgi_args.push_back(strdup((std::string("SERVER_SOFTWARE") + "=Webserv 1.0").c_str()));
	cgi_args.push_back(strdup((std::string("SERVER_NAME") + "="+location.host).c_str()));
	cgi_args.push_back(strdup((std::string("PATH_TRANSLATED") + "="+req_file).c_str()));
	cgi_args.push_back(strdup((std::string("SERVER_PORT") + "="+to_string(location.port)).c_str()));
	cgi_args.push_back(strdup((std::string("HTTP_COOKIE") + "="+cookies).c_str()));
	cgi_args.push_back(strdup((std::string("PATH_INFO") + "=?"+params).c_str()));
	cgi_args.push_back(strdup((std::string("QUERY_STRING") + "="+ params).c_str()));
	cgi_args.push_back(strdup((std::string("REQUEST_METHOD") + "="+req_method).c_str()));
	cgi_args.push_back(strdup((std::string("CONTENT_TYPE") + "="+req_content_type).c_str()));
	cgi_args.push_back(strdup((std::string("CONTENT_LENGTH") + "="+req_content_length).c_str()));
	cgi_args.push_back(NULL);
}

std::string Cgi_class::to_string(int n)
{
	std::ostringstream string;
	string << n;
	return string.str();
}