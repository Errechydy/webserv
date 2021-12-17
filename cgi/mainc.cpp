#include "cgi_class.cpp"


int main(int argc, char **argvg, char **env)
{


    Location loc;
    loc.cgi_path = "/goinfre/ler-rech/.brew/bin/php-cgi";
    std::string file = "/Users/ler-rech/Desktop/webserv/cgi/php.php";

    Cgi_class cgi(loc, file);
    



    std::string cgi_body = "v=900fs55";
    cgi.send_cgi_body(cgi_body);

    

    int nbytes;
	char cgi_buff[1024] = {0};
    while ((nbytes = read(cgi.pipe_fd[0], cgi_buff, 1024)) > 0) { 
            std::cout << cgi_buff << std::endl;
    }
	close(cgi.pipe_fd[0]); // close the read end after finishing reading, don't close it until you finish reading all cgi response (EOF or read content_length if the response has content_length)


    // file descriptor leaks => lsof ‑a ‑p 95839 ‑d *
    while(1)
    {

    }

    return (0);
}