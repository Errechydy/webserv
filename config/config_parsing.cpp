#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>

struct Location {
    std::string         location_name;          // /, /page/ , ....
    std::string         error_page;             // exp: error.html
    int                 client_max_body_size;   // in kb,  -1 unlimited,
    std::string         method;                 // GET, POST, DELETE
    std::string         redirect;               // /abc/ef/
    std::string         root;                   // /www/html/
    int                 auto_index;             // 0, 1
    std::string         default_answer;         // index.html
    std::string         cgi_extension;          // php
    std::string         cgi_path;               // localhost:9000
    int                 accept_upload;          // 0, 1
    std::string         upload_path;            // /www/html/upload
};

struct Server {
    int                 port;
    std::string         host;
    std::string         server_name;
    std::string         error_page;             // exp: error.html
    std::map<std::string, Location>  locations;
};

struct Config {
    int                 port;
    std::string         host;
    std::vector<Server> servers; 
};





class	Config_parser
{

	public:
        std::string config_file;
        Config      config_info;
	public:
		explicit Config_parser(std::string cf) : config_file(cf) {
            int parsing;
            parsing = parse_config_file();
            if(parsing > 0)
                std::cout << "Error in config file" << std::endl;

        }
		~Config_parser() {
            // free config object
        }

        int parse_config_file()
        {
            std::string str;
            std::ifstream cnfg_file(config_file);
            std::string tmp_arg;
            bool server_active = false;
            bool location_active = false;
            std::string location_name;

            int server_num = -1;

            while(getline(cnfg_file, str))
            {
                if(empty_line(str))
                    continue;
                if(!valid_arg(str))
                    return (1);

                std::cout << str << std::endl;

                tmp_arg = get_arg(str);
                if(tmp_arg == "server")
                {
                    server_num++;
                    server_active = true;
                    config_info.servers[server_num] = new_server();
                }
                else if(tmp_arg == "listen")
                {
                    config_info.port = get_port(str);
                    config_info.servers[server_num].port = config_info.port;
                }
                else if(tmp_arg == "host")
                {
                    config_info.host = get_host(str);
                    config_info.servers[server_num].host = config_info.host;
                }
                else if(tmp_arg == "server_name")
                {
                    config_info.servers[server_num].server_name = get_server_name(str);
                }
                else if(tmp_arg == "location")
                {
                    if(location_active)
                        return (1);
                    location_active = true;
                    location_name = get_location_name(str);
                    config_info.servers[server_num].locations[location_name] = new_location();
                }
                else if(tmp_arg == "error_page")
                {
                    if(location_active)
                        config_info.servers[server_num].locations[location_name].error_page = new_error_page(str);
                    else if(server_active)
                        config_info.servers[server_num].error_page = new_error_page(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "client_max_body_size")
                {

                }
                else if(tmp_arg == "method")
                {

                }
                else if(tmp_arg == "redirect")
                {

                }
                else if(tmp_arg == "root")
                {

                }
                else if(tmp_arg == "autoindex")
                {

                }
                else if(tmp_arg == "default_answer")
                {

                }
                else if(tmp_arg == "cgi_extension")
                {

                }
                else if(tmp_arg == "cgi_path")
                {

                }
                else if(tmp_arg == "accept_upload")
                {

                }
                else if(tmp_arg == "upload_path")
                {

                }
                else if(tmp_arg == "end_of_block")
                {
                    if(location_active)
                        location_active = false;
                    else if(server_active)
                        server_active = false;
                }
            }
            cnfg_file.close();

            if(server_active || location_active)
                return (1);
            return 0;
        }

        bool empty_line(std::string str)
        {
            int i;
            i = 0;
            while(str[i] != '\0')
            {
                if(!(str[i] == 32 || str[i] == 9 || str[i] == 11))
                    return false;
                i++;
            }
            return true;
        }

        std::string get_arg(std::string str)
        {
            if(str.find("server") != std::string::npos)
                return "server";
            else if(str.find("listen") != std::string::npos)
                return "listen";
            else if(str.find("location") != std::string::npos)
                return "location";
            else if(str.find("host") != std::string::npos)
                return "host";
            else if(str.find("server_name") != std::string::npos)
                return "server_name";
            else if(str.find("error_page") != std::string::npos)
                return "error_page";
            else if(str.find("client_max_body_size") != std::string::npos)
                return "client_max_body_size";
            else if(str.find("method") != std::string::npos)
                return "method";
            else if(str.find("redirect") != std::string::npos)
                return "redirect";
            else if(str.find("root") != std::string::npos)
                return "root";
            else if(str.find("autoindex") != std::string::npos)
                return "autoindex";
            else if(str.find("default_answer") != std::string::npos)
                return "default_answer";
            else if(str.find("cgi_extension") != std::string::npos)
                return "cgi_extension";
            else if(str.find("cgi_path") != std::string::npos)
                return "cgi_path";
            else if(str.find("accept_upload") != std::string::npos)
                return "accept_upload";
            else if(str.find("upload_path") != std::string::npos)
                return "upload_path";
            else
                return "end_of_block";
        }
        bool valid_arg(std::string str)
        {
            if(str.find("server") != std::string::npos ||
                str.find("listen") != std::string::npos ||
                str.find("location") != std::string::npos ||
                str.find("server_name") != std::string::npos ||
                str.find("host") != std::string::npos ||
                str.find("error_page") != std::string::npos ||
                str.find("client_max_body_size") != std::string::npos ||
                str.find("method") != std::string::npos ||
                str.find("redirect") != std::string::npos ||
                str.find("root") != std::string::npos ||
                str.find("autoindex") != std::string::npos ||
                str.find("default_answer") != std::string::npos ||
                str.find("cgi_extension") != std::string::npos ||
                str.find("cgi_path") != std::string::npos ||
                str.find("accept_upload") != std::string::npos ||
                str.find("upload_path") != std::string::npos ||
                str.find("}") != std::string::npos)
                return true;
            return false;
        }

        Server new_server()
        {
            Server server;
            std::map<std::string, Location>  location;

            server.server_name = "";
            server.port = 80;
            server.host = "";

            return server;
        }

        Location new_location()
        {
            Location location;

            location.method = "GET";
            location.client_max_body_size = -1; // unlimited
            location.accept_upload = 0;
            location.default_answer = "";
            location.error_page = "";
            location.auto_index = 0;
            location.redirect = "";
            location.root = "";
            location.cgi_extension = "";
            location.cgi_path = "";
            location.upload_path = "";

            return location;
        }

        int get_port(std::string str)
        {
            size_t i = 0;
            for ( ; i < str.length(); i++ ){
                if (isdigit(str[i]))
                    break;
            }
            str = str.substr(i, str.length() - i );
            int port = atoi(str.c_str());
            return port;
        }

        std::string get_host(std::string str)
        {
            return "";
        }

        std::string get_server_name(std::string str)
        {
            return "";
        }

        std::string get_location_name(std::string str)
        {
            return "";
        }
};



