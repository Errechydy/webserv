#ifndef SocketData_hpp
#define SocketData_hpp

#include "ResponseBuilder.hpp"
#include "ConfigFile.hpp"
#include "Tools.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <map>

#define Black "\033[0;30m"
#define Red "\033[0;31m"
#define Green "\033[0;32m"
#define Yellow "\033[0;33m"
#define Blue "\033[0;34m"
#define Purple "\033[0;35m"
#define Cyan "\033[0;36m"
#define White "\033[0;37m"
#define Reset "\033[0m"

class SocketData{
    friend class MySocket;

    public:
        typedef std::map<std::string, std::string>  reqMap;
        typedef std::pair<std::string, std::string> reqPair;

    private:
        bool                                _dataReadCheck;
        bool                                _recheckHeader;
        bool                                _recheckbody;
        size_t                              _dataWriteCheck;
        size_t                              _reqHeaderLen;
        size_t                              _reqbodyLen;
        int                                 _server;
        std::string                         _request;
        std::string                         _response;
        std::string                         _reqBody;
        reqMap                              _requestParssed;

    public:
        SocketData();
    
    private:
        void append(const char* buffer, int byteRecived);
        void checkDataReadEnd();
        void parse();
        void buildResponse(const Config_parser &config, const Tools &tools, int server);
        void check_chunked();
        void check_query();
};

#endif