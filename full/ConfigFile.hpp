#ifndef CONFIG_PARSING_HPP
# define CONFIG_PARSING_HPP

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
    int                 port;
    std::string         host;
    std::string         server_name;
    std::map<std::string, std::string>  error_page;
    std::string         client_max_body_size;
    std::string         method;
    std::map<std::string, std::string>         redirect;
    std::string         root;
    std::string         autoindex;
    std::string         default_answer;
    std::string         cgi_extension;
    std::string         cgi_path;
    std::string         accept_upload;
    std::string         upload_path;
};

struct Server {
    int                 port;
    std::string         host;
    std::string         server_name;
    std::map<std::string, std::string> error_page;
    std::string         client_max_body_size;
    std::string         method;
    std::map<std::string, std::string>         redirect;
    std::string         root;
    std::string         autoindex;
    std::string         default_answer;
    std::string         cgi_extension;
    std::string         cgi_path;
    std::string         accept_upload;
    std::string         upload_path;
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
        std::vector<Config>         config_info;
        std::string                 config_file;
	public:
		explicit Config_parser(std::string cf);
		~Config_parser();

		Location get_location_info(int conf_index, std::string url);
		Location get_location_info_from_server(int conf_index, int server_index, std::string url);
		bool valid_location(std::string location_name, std::string url);
		int slashes_len(std::string str);
        int parse_config_file();
        bool empty_line(std::string str);
        std::string get_arg(std::string str);
        bool arg_found(std::string str, std::string arg);
        bool valid_arg(std::string str);
        Config new_config();
        Server new_server();
        Location new_location();
        int get_port(std::string str);
        std::string get_location_name(std::string str);
        std::pair<std::string , std::string> get_error_value(std::string str);
        std::string get_value(std::string str);
        void fill_locations ();
        void order_servers ();
};


#endif