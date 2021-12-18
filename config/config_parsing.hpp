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

struct Server {
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
		explicit Config_parser(std::string cf) : config_file(cf) {
            int parsing;
            parsing = parse_config_file();
            if(parsing > 0)
                std::cout << "Error in config file" << std::endl;
            fill_locations();
            order_servers();
        }
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
        std::string get_value(std::string str);
        void fill_locations ();
        void order_servers ();
};


#endif
