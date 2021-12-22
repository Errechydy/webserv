#ifndef tools_hpp
#define tools_hpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

class Tools{
    friend class ResponseBuilder;

    public:
        typedef std::map<std::string, std::string> mimeMap;
    
    private:
        mimeMap _mime;

    public:
        Tools();
    
    private:
        mimeMap& setMime();
};

#endif