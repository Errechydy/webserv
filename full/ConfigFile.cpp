#include "ConfigFile.hpp"

Config_parser::Config_parser(std::string cf) : config_file(cf) {
    int parsing;
    parsing = parse_config_file();
    if(parsing > 0)
        std::cout << "Error in config file" << std::endl;
    fill_locations();
    order_servers();
}

Config_parser::~Config_parser(){
}

Location Config_parser::get_location_info(int conf_index, std::string url)
{
    url = url.substr(0, url.find("?"));


    // std::cout << "URL = " << url << std::endl;



    std::vector<Server> servers = config_info[conf_index].servers;
    int server_index = 0;

    for (int i = servers.size() - 1; i >= 0; i--)
    {
        // std::cout << "Server name : " << servers[i].server_name << std::endl;



        // std::cout << "comparing => " << url << " : " << servers[i].server_name << std::endl;
        if(url.find(servers[i].server_name) != std::string::npos) // TODO: select the server index for this url
        {
            server_index = i;
            // std::cout << "sn = " << servers[i].server_name << std::endl;
        }
    }
    return get_location_info_from_server(conf_index, server_index, url);
}

Location Config_parser::get_location_info_from_server(int conf_index, int server_index, std::string url)
{
    Location location;
    std::string old_lacation_name = "";


    // std::cout << "URL = " << url << std::endl;


    std::map<std::string, Location>::iterator it;

    for (it = config_info[conf_index].servers[server_index].locations.begin(); it != config_info[conf_index].servers[server_index].locations.end(); it++)
    {
        if(valid_location(it->first, url) && slashes_len(it->first) >= slashes_len(old_lacation_name) && it->first.size() >= old_lacation_name.size())
        {
            old_lacation_name = it->first;
            location = it->second;
        }
    }

    // std::cout << "loc cgi = " << location.cgi_path << std::endl;
    // std::cout << "loc extension = " << location.cgi_extension << std::endl;


    return location;
}

bool Config_parser::valid_location(std::string location_name, std::string url) // url without server_name
{
    // ex: location_name = / , url = /dshfisdf/sdfds ===> true // start with : url.find(location_name) != std::string::npos
    if(url.find(location_name) != std::string::npos)
        return true;
    return false;
}

int Config_parser::slashes_len(std::string str)
{
    int i = 0;
    int len = 0;
    while(str[i])
    {
        if(str[i] == 47)
            len++;
        i++;
    }
    return len;
}

int Config_parser::parse_config_file()
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
                config_info[config_num].servers[0].locations[location_name].error_page.insert(get_error_value(str));
            else if(server_active)
                config_info[config_num].servers[0].error_page.insert(get_error_value(str)); 
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

bool Config_parser::empty_line(std::string str)
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

std::string Config_parser::get_arg(std::string str)
{

    if(arg_found(str, "server_name ") || arg_found(str, "server_name	"))
        return "server_name";
    if(arg_found(str, "cgi_path ") || arg_found(str, "cgi_path	"))
        return "cgi_path";
    if(arg_found(str, "default_answer ") || arg_found(str, "default_answer	"))
        return "default_answer";
    if(arg_found(str, "server ") || arg_found(str, "server	"))
        return "server";
    if(arg_found(str, "listen ") || arg_found(str, "listen	"))
        return "listen";
    if(arg_found(str, "location ") || arg_found(str, "location	"))
        return "location";
    if(arg_found(str, "host ") || arg_found(str, "host	"))
        return "host";
    if(arg_found(str, "error_page ") || arg_found(str, "error_page	"))
        return "error_page";
    if(arg_found(str, "client_max_body_size ") || arg_found(str, "client_max_body_size	"))
        return "client_max_body_size";
    if(arg_found(str, "method ") || arg_found(str, "method	"))
        return "method";
    if(arg_found(str, "redirect ") || arg_found(str, "redirect	"))
        return "redirect";
    if(arg_found(str, "root ") || arg_found(str, "root	"))
        return "root";
    if(arg_found(str, "autoindex ") || arg_found(str, "autoindex	"))
        return "autoindex";
    if(arg_found(str, "cgi_extension ") || arg_found(str, "cgi_extension	"))
        return "cgi_extension";
    if(arg_found(str, "accept_upload ") || arg_found(str, "accept_upload	"))
        return "accept_upload";
    if(arg_found(str, "upload_path ") || arg_found(str, "upload_path	"))
        return "upload_path";
    else
        return "end_of_block";
}
bool Config_parser::arg_found(std::string str, std::string arg)
{
    int i = 0;
    int len = 0;
    int arg_len = arg.size();

    while(str[i])
    {
        if(str[i] != 32 && str[i] != 9)
            break ;
        i++;
    }
    while(str[i])
    {
        if(str[i] == arg[len])
            len++;
        else
            break ;
        i++;
    }
    if(arg_len == len)
        return true;
    return false;

}
bool Config_parser::valid_arg(std::string str)
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

Config Config_parser::new_config()
{
    Config conf;
    conf.port = 80;
    conf.host = "";
    conf.servers = std::vector<Server>();
    return conf;
}

Server Config_parser::new_server()
{
    Server server;
    // std::map<std::string, Location>  location;

    server.port = 80;
    server.method = "";
    // server.error_page.insert(std::pair<std::string , std::string>("404", "404.html"));
    server.accept_upload = "0";
    server.autoindex = "0";

    return server;
}

Location Config_parser::new_location()
{
    Location location;

    return location;
}

int Config_parser::get_port(std::string str)
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

std::string Config_parser::get_location_name(std::string str)
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

std::pair<std::string , std::string> Config_parser::get_error_value(std::string str)
{
    std::string v = get_value(str);
    std::string value = get_value(v);
    std::string key = v.substr(0, v.find(" "));

    return std::pair<std::string , std::string>(key, value);
}

std::string Config_parser::get_value(std::string str)
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

void Config_parser::fill_locations ()
{
    size_t i = 0;
    std::map<std::string, Location>::iterator it;

    while(i < config_info.size())
    {
        for (it = config_info[i].servers[0].locations.begin(); it != config_info[i].servers[0].locations.end(); it++)
        {
            it->second.port = config_info[i].servers[0].port;
            it->second.host = config_info[i].servers[0].host;
            it->second.server_name = config_info[i].servers[0].server_name;

            // if(it->second.error_page == "")
            //     it->second.error_page = config_info[i].servers[0].error_page;


            it->second.error_page.insert(config_info[i].servers[0].error_page.begin(), config_info[i].servers[0].error_page.end());




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

void Config_parser::order_servers ()
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
