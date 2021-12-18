#include "config_parsing.cpp"


int main()
{
    std::string config_file = "1.config";

    Config_parser config(config_file);








    Location loc = config.get_location_info(0, "localhost:8080/sfds/hello");
    // Location loc = config.get_location_info(0, "127.0.0.1:8080/php/hello");

    std::cout << loc.accept_upload << std::endl; 
    
    return (0);

}