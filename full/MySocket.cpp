#include "MySocket.hpp"

MySocket::MySocket(const Config_parser &config, const Tools &tools) : _config(config), _tools(tools){

    for (size_t i = 0; i < _config.config_info.size(); i++){
        Server server;
        _server.push_back(server);
        _server[i]._socketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (_server[i]._socketFd < 0){
            perror("Socket creation fieled");
            exit(EXIT_FAILURE);
        }
        socketBind(i, _server[i]._socketFd, _config.config_info[i].port);
        socketListen(_server[i]._socketFd);
    }
    newSocketHandler();
}

void MySocket::socketBind(const int &i, const int &socket, const int &port){
    _server[i]._servAdd.sin_family = AF_INET;
    _server[i]._servAdd.sin_addr.s_addr = (_config.config_info[i].host == "" || _config.config_info[i].host == "localhost") ? INADDR_ANY : inet_addr(_config.config_info[i].host.c_str());
    _server[i]._servAdd.sin_port = htons(port);
    memset(_server[i]._servAdd.sin_zero, '\0', sizeof(_server[i]._servAdd.sin_zero));
    if (bind(socket, (struct sockaddr *)&(_server[i]._servAdd), sizeof(_server[i]._servAdd)) < 0)
    {
        perror("Socket can't bind:");
        exit(EXIT_FAILURE);
    }
}

void MySocket::socketListen(const int &socket){
    if (listen(socket, SOMAXCONN) < 0)
    {
        perror("Socket can't listen:");
        exit(EXIT_FAILURE);
    }
}

int MySocket::socketAccept(const int &socket){
    int                 new_socket;
    struct sockaddr_in  newSocketAdd;
    socklen_t           newSockAddLen = sizeof(newSocketAdd);
    
    if ((new_socket = accept(socket, (struct sockaddr *)&newSocketAdd, (socklen_t*)&newSockAddLen))<0)
    {
        perror("Socket Can't accept connections:");
        exit(EXIT_FAILURE);
    }
    return new_socket;
}

void    MySocket::newSocketHandler(){
    int newSocket;
    int fd_max = _server[_server.size() - 1]._socketFd;
    std::map<int, int> master_servers;
    std::map<int, int>::iterator it;

    FD_ZERO(&_readFds);
    FD_ZERO(&_masterFds);
    FD_ZERO(&_writeFds);
    for (size_t i = 0; i < _server.size(); i++){
        FD_SET(_server[i]._socketFd, &_masterFds);
        master_servers.insert(std::pair<int, int>(_server[i]._socketFd, i));
    }
    
    while(1){
        _readFds = _masterFds;
        if (select(fd_max + 1, &_readFds, &_writeFds, NULL, NULL) < 0){
            perror("select fd probleme:");
            exit(EXIT_FAILURE);
        }
        for(int i = 0; i <= fd_max; i++){
            if (FD_ISSET(i, &_readFds)){
                it = master_servers.find(i);
                if (it != master_servers.end()){
                    newSocket = socketAccept(i);
                    FD_SET(newSocket, &_masterFds);
                    fd_max = newSocket > fd_max ? newSocket : fd_max;
                    _sockData.insert(std::pair<int, SocketData>(newSocket, SocketData())).first->second._server = it->second;
                } else {
                    dataHandler(i);
                }
            }
            else if (FD_ISSET(i, &_writeFds)){
                sendResponse(i);
            }
        }
    }
}

void    MySocket::sendResponse(int newSocket){
    reqMap::iterator tmp;
    dataMap::iterator it = _sockData.find(newSocket);
    int check = send(it->first, &it->second._response[0], it->second._response.size(), 0);
    if (check < 0){
        FD_CLR(it->first, &_writeFds);
        FD_CLR(it->first, &_masterFds);
        close(it->first);
        _sockData.erase(it->first);
        return;
    }
    it->second._dataWriteCheck += check;
    if (it->second._dataWriteCheck == it->second._response.size()){
        FD_CLR(it->first, &_writeFds);
        FD_CLR(it->first, &_masterFds);
        tmp = it->second._requestParssed.find("Connection");
        if (tmp != it->second._requestParssed.end() && tmp->second == "close"){ 
            close(it->first);
        }
        _sockData.erase(it->first);
    }
}

void     MySocket::dataHandler(const int &newSocket){
    int         byteRcived;
    char        buffer[1024] = {'\0'};
    byteRcived = recv(newSocket, &buffer[0], sizeof(buffer), 0);
    if (byteRcived <= 0){
        if (byteRcived == 0){
        } else {
            perror("Error reciving data from some new socket:");
        }
        FD_CLR(newSocket, &_readFds);
        FD_CLR(newSocket, &_writeFds);
        close(newSocket);
        FD_CLR(newSocket, &_masterFds);
        _sockData.erase(newSocket);
    } else {
        dataMap::iterator it = _sockData.find(newSocket);
        it->second.append(buffer, byteRcived);
        if (it->second._dataReadCheck){
            FD_CLR(it->first, &_readFds);
            FD_SET(it->first, &_writeFds);
            it->second.parse();
            it->second.buildResponse(_config, _tools, it->second._server);
        }
    }
}