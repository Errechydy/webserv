#include "MySocket.hpp"
#include "ConfigFile.hpp"
#include "Tools.hpp"

int main(){
    std::string config_file = "./files/config.conf";

    Config_parser   config(config_file);

    //std::cout << config.config_info[0].port << std::endl;
    Tools           tools;
    MySocket        socket(config, tools);
    //system("leaks webserv");

    return 0;
}