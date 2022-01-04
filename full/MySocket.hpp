#ifndef socket_hpp
#define socket_hpp

#include <sys/types.h>//socket
#include <sys/socket.h>//socket
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>//sockaddr_in
#include <netinet/tcp.h>
#include <arpa/inet.h>//inet_ntoa
#include <map>
#include "SocketData.hpp"
#include "ConfigFile.hpp"
#include "Tools.hpp"

#define Black "\033[0;30m"
#define Red "\033[0;31m"
#define Green "\033[0;32m"
#define Yellow "\033[0;33m"
#define Blue "\033[0;34m"
#define Purple "\033[0;35m"
#define Cyan "\033[0;36m"
#define White "\033[0;37m"
#define Reset "\033[0m"

class MySocket{

    public:
        typedef std::map<int, SocketData> dataMap;
        typedef std::map<std::string, std::string>  reqMap;

    private:
        struct Server{
            struct sockaddr_in          _servAdd;
            uint16_t                    _socketFd;
        };
        std::vector<Server>             _server;              
        //uint16_t                    _port;
        std::string                 _root;
        fd_set                      _readFds;
        fd_set                      _writeFds;
        fd_set                      _masterFds;
        //std::map<int, std::string>  _sockRequests;
        dataMap                     _sockData;
        Config_parser               _config;
        Tools                       _tools;

    public:
        MySocket(const Config_parser &config, const Tools &tools);
    
    private:
        void socketBind(const int &i, const int &socket, const int &port);
        void socketListen(const int &socket);
        int  socketAccept(const int &socket);
        void newSocketHandler();
        void dataHandler(const int &newSocket);
        void sendResponse(int newSocket);


        
        //void requestParssing(dataMap::iterator &it);
        //void buildResponse(reqMap::iterator &it);
};

#endif