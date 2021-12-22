#include "cgi_class.cpp"


int main(int argc, char **argvg, char **env)
{


    Location loc;
    loc.cgi_path = "/goinfre/ler-rech/.brew/bin/php-cgi";
    loc.method = "POST";
    loc.port = 80;
    std::string file = "/Users/ler-rech/Desktop/webserv/cgi/php.php";
    std::string cookies = "";

    Cgi_class cgi(loc, file, cookies);
    


// Set-Cookie


    std::string cgi_body = "v=900fs55";
    cgi.send_cgi_body(cgi_body);

    int nbytes;
	char cgi_buff[1024] = {0};
    while ((nbytes = read(cgi.pipe_fd[0], cgi_buff, 1024)) > 0) { 
           std::cout << cgi_buff << std::endl;
    }
	close(cgi.pipe_fd[0]); // close the read end after finishing reading, don't close it until you finish reading all cgi response (EOF or read content_length if the response has content_length)


    // file descriptor leaks => lsof ‑a ‑p 95839 ‑d *
    // while(1)
    // {

    // }


    ////////////////
// file 2 //////
    ////////////////



std::cout << std::endl << std::endl<< std::endl << std::endl;
std::string cgi_buffsfds(cgi_buff);
std::string sessionInfo = cgi_buffsfds.substr(37,45);
// std::cout << "===> " << sessionInfo << std::endl;


std::string file2 = "/Users/ler-rech/Desktop/webserv/cgi/php2.php";
Cgi_class cgi2(loc, file2, sessionInfo);
std::string cgi_body2 = "v=900fs55";
cgi2.send_cgi_body(cgi_body2);
int nbytes2;
char cgi_buff2[1024] = {0};
while ((nbytes2 = read(cgi.pipe_fd[0], cgi_buff2, 1024)) > 0) { 
        std::cout << cgi_buff2 << std::endl;
}
close(cgi.pipe_fd[0]); // close the read end after finishing reading, don't close it until you finish reading all cgi response (EOF or read content_length if the response has content_length)






    return (0);
}