#ifndef response_builder_hpp
#define response_builder_hpp

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include<dirent.h>
#include <ctime>
#include "SocketData.hpp"
#include "ConfigFile.hpp"
#include "Cgi.hpp"
#include "Tools.hpp"


class ResponseBuilder{
    friend class SocketData;

    public:
        typedef std::map<std::string, std::string>  reqMap;

    private:
        const reqMap                &_request;
        Config_parser               _config;
        const Tools                 &_tools;
        Location                    _location;
        std::string                 _response;
        std::string                 _status_code;
        std::string                 _body;
        std::string                 _red_location;
    
    public:
        ResponseBuilder(const reqMap &request, Config_parser config, const Tools &tools, int server);

    private:
        void    check_http(const std::string &http);
        void    check_method(const std::string &method, const std::string &allowed_methods);
        void    check_uri(const std::string &uri, const std::string &root, const std::string &indexs);
        void    check_content_type(const std::string &extention);
        int     check_indexs(std::string &path, const std::string &indexs);
        int     check_autoindex(const std::string &path);
        int     check_uploads();
        int     check_max_body();
        void    check_cgi(const std::string &path);
        void    redirection(const std::string &path);
        void    build_response();
        void    stream_body(std::string &body, const std::string &path);

        std::string    get_time();
        std::string    last_mod(const time_t &lmod);
};

#endif