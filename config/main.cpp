#include "config_parsing.cpp"


int main()
{
    std::string config_file = "1.config";

    Config_parser config(config_file);


    std::cout << config.config_info[0].port << std::endl;
    

}