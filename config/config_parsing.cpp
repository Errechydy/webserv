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
    std::string         error_page;             // exp: error.html
    std::string         client_max_body_size;   // in kb,  -1 unlimited,
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
    std::string         client_max_body_size;   // in kb,  -1 unlimited,
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

            int config_num = -1;

            while(getline(cnfg_file, str))
            {
                if(empty_line(str))
                    continue;
                if(!valid_arg(str))
                    return (1);

                // std::cout << str << std::endl;

                tmp_arg = get_arg(str);
                
                if(tmp_arg == "server")
                {
                    server_active = true;
                    config_num++;
                    config_info.push_back(new_config());
                    config_info[config_num].servers.push_back(new_server());
                }
                else if(tmp_arg == "listen")
                {
                    config_info[config_num].port = get_port(str);
                    config_info[config_num].servers[0].port = config_info[config_num].port;
                    config_info[config_num].servers[0].port = config_info[config_num].port;
                }
                else if(tmp_arg == "host")
                {
                    config_info[config_num].host = get_value(str);
                    config_info[config_num].servers[0].host = config_info[config_num].host;
                }
                else if(tmp_arg == "server_name")
                {
                    config_info[config_num].servers[0].server_name = get_value(str);
                }
                else if(tmp_arg == "location")
                {
                    if(location_active)
                        return (1);
                    location_active = true;
                    location_name = get_location_name(str);
                    config_info[config_num].servers[0].locations[location_name] = new_location();
                }
                else if(tmp_arg == "error_page")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].error_page = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].error_page = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "client_max_body_size")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].client_max_body_size = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].client_max_body_size = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "method")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].method = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].method = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "redirect")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].redirect = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].redirect = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "root")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].root = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].root = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "autoindex")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].autoindex = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].autoindex = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "default_answer")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].default_answer = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].default_answer = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "cgi_extension")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].cgi_extension = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].cgi_extension = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "cgi_path")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].cgi_path = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].cgi_path = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "accept_upload")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].accept_upload = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].accept_upload = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "upload_path")
                {
                    if(location_active)
                        config_info[config_num].servers[0].locations[location_name].upload_path = get_value(str);
                    else if(server_active)
                        config_info[config_num].servers[0].upload_path = get_value(str);
                    else
                        return (1);
                }
                else if(tmp_arg == "end_of_block")
                {
                    if(location_active)
                        location_active = false;
                    else if(server_active)
                        server_active = false;
                    else
                        return (1);
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
            if(str.find("server_name") != std::string::npos)
                return "server_name";
            else if(str.find("cgi_path") != std::string::npos)
                return "cgi_path";
            else if(str.find("default_answer") != std::string::npos)
                return "default_answer";
            else if(str.find("server") != std::string::npos)
                return "server";
            else if(str.find("listen") != std::string::npos)
                return "listen";
            else if(str.find("location") != std::string::npos)
                return "location";
            else if(str.find("host") != std::string::npos)
                return "host";
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
            else if(str.find("cgi_extension") != std::string::npos)
                return "cgi_extension";
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

        Config new_config()
        {
            Config conf;
            conf.port = 80;
            conf.host = "";
            conf.servers = std::vector<Server>();
            return conf;
        }

        Server new_server()
        {
            Server server;
            // std::map<std::string, Location>  location;

            server.port = 80;
            server.method = "GET";
            server.error_page = "404.html";
            server.client_max_body_size = "-1"; // unlimited
            server.accept_upload = "0";
            server.autoindex = "0";

            return server;
        }

        Location new_location()
        {
            Location location;

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

        std::string get_location_name(std::string str)
        {
            std::string location_name;
            size_t pos = str.find("location");
            int i = 0;
            int j = pos + 9;
            while(str[j])
            {
                if(str[j] == 32)
                    break ;
                i++;
                j++;
            }
            location_name = str.substr(pos + 9, i);
            return location_name;
        }

        std::string get_value(std::string str)
        {
            std::string value;

            int last = str.size() - 1;
            while (last >= 0)
            {
                if(str[last] != 32 && str[last] != 9)
                    break ;
                --last;
            }
            value = str.substr(0, last + 1);
            int len = value.size();
            int first = 0;
            while (value[first])
            {
                if(value[first] != 32 && value[first] != 9)
                    break ;
                first++;
            }
            value = value.substr(first, len);
            value = value.substr(value.find(" ") + 1);
            value = value.substr(0, value.find(";"));
            last = value.size() - 1;
            while (last >= 0)
            {
                if(value[last] != 32 && value[last] != 9)
                    break ;
                --last;
            }
            value = value.substr(0, last + 1);
            len = value.size();
            first = 0;
            while (value[first])
            {
                if(value[first] != 32 && value[first] != 9)
                    break ;
                first++;
            }
            value = value.substr(first, len);
            return value;
        }

        void fill_locations ()
        {
            int i = 0;
            std::map<std::string, Location>::iterator it;

            while(i < config_info.size())
            {
                for (it = config_info[i].servers[0].locations.begin(); it != config_info[i].servers[0].locations.end(); it++)
                {
                    if(it->second.error_page == "")
                        it->second.error_page = config_info[i].servers[0].error_page;
                    if(it->second.client_max_body_size == "")
                        it->second.client_max_body_size = config_info[i].servers[0].client_max_body_size;
                    if(it->second.method == "")
                        it->second.method = config_info[i].servers[0].method;
                    if(it->second.redirect == "")
                        it->second.redirect = config_info[i].servers[0].redirect;
                    if(it->second.root == "")
                        it->second.root = config_info[i].servers[0].root;
                    if(it->second.autoindex == "")
                        it->second.autoindex = config_info[i].servers[0].autoindex;
                    if(it->second.default_answer == "")
                        it->second.default_answer = config_info[i].servers[0].default_answer;
                    if(it->second.cgi_extension == "")
                        it->second.cgi_extension = config_info[i].servers[0].cgi_extension;
                    if(it->second.cgi_path == "")
                        it->second.cgi_path = config_info[i].servers[0].cgi_path;
                    if(it->second.accept_upload == "")
                        it->second.accept_upload = config_info[i].servers[0].accept_upload;
                    if(it->second.upload_path == "")
                        it->second.upload_path = config_info[i].servers[0].upload_path;
                }
                i++;
            }
        }

        void order_servers ()
        {
            int len = config_info.size();
            int i = 0;
            int j;

            while(i < len)
            {
                j = i + 1;
                while(j < len)
                {
                    if(config_info[i].port == config_info[j].port && config_info[i].host == config_info[j].host)
                    {
                        config_info[i].servers.push_back(config_info[j].servers[0]);
                        config_info.erase(config_info.begin() + j);
                        len--;
                    }
                    j++;
                }
                i++;
            }
        }
};



