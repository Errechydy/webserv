#include "MySocket.hpp"
#include "ConfigFile.hpp"
#include "Tools.hpp"

int main(){
    std::string config_file = "./files/config.conf";

    Config_parser   config(config_file);
    Tools           tools;
    MySocket        socket(config, tools);

    return 0;
}