#include "Tools.hpp"

Tools::Tools(){
    _mime.clear();
    setMime();
}

Tools::mimeMap& Tools::setMime(){
    std::ifstream input("./files/mime.csv");
    std::string     line;
    std::string buffer, key, value;
    while (std::getline(input, line))
    {
        std::istringstream line_strem(line);
        int i = 0;
        while(std::getline(line_strem, buffer, ',' ) && i < 2){
            if (i == 0)
                key = buffer;
            else if (i == 1)
                value = buffer;
            i++;
        }
        _mime.insert(std::pair<std::string, std::string>(key, value));
        key.clear();
        value.clear();
        buffer.clear();
        line_strem.clear();
    }
    input.close();
    return _mime;
}