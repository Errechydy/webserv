#include "MySocket.hpp"

MySocket::MySocket(const Config_parser &config, const Tools &tools) : _config(config), _tools(tools){
    //parse config file
    //Config_parser config;
    //init value
    // _port = config._port;
    // _root = config._root;
    //int flags =1;
    
    //socket creation
    // std::cout << "number of server: " << _config.config_info.size() << std::endl;
    //_server.reserve(_config.config_file.size());
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
    // flags = 10;
    // if (setsockopt(_socketFd, SOL_SOCKET, SO_KEEPIDLE, (void *)&flags, sizeof(flags))) { perror("ERROR: setsocketopt(), SO_KEEPIDLE"); exit(0); };

    // int val = 1;
    // setsockopt(_socketFd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));
    // flags = 5;
    // if (setsockopt(_socketFd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&flags, sizeof(flags))) { perror("ERROR: setsocketopt(), SO_KEEPCNT"); exit(0); };

    // flags = 5;
    // if (setsockopt(_socketFd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&flags, sizeof(flags))) { perror("ERROR: setsocketopt(), SO_KEEPINTVL"); exit(0); };
    newSocketHandler();
    //std::cout << "socket id:" << _socketFd << std::endl;
}

void MySocket::socketBind(const int &i, const int &socket, const int &port){
    _server[i]._servAdd.sin_family = AF_INET;
    _server[i]._servAdd.sin_addr.s_addr = INADDR_ANY;
    _server[i]._servAdd.sin_port = htons(port);
    memset(_server[i]._servAdd.sin_zero, '\0', sizeof(_server[i]._servAdd.sin_zero));
    if (bind(socket, (struct sockaddr *)&(_server[i]._servAdd), sizeof(_server[i]._servAdd)) < 0)
    {
        perror("Socket can't bind:");
        exit(EXIT_FAILURE);
    }
    // std::cout << "socket id:" << socket;
    // std::cout << " binded on port: " << port << std::endl;
}

void MySocket::socketListen(const int &socket){
    if (listen(socket, 10) < 0)
    {
        perror("Socket can't listen:");
        exit(EXIT_FAILURE);
    }
    // std::cout << "listning on socket " << socket << std::endl;
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
    //std::cout << "Recive connection from Address: " << inet_ntoa(newSocketAdd.sin_addr) << std::endl;
    
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
        //std::cout << "waiting for new connections on port " << _port << "..."<< std::endl;
        _readFds = _masterFds;
        //_writeFds = _masterFds;
        if (select(fd_max + 1, &_readFds, &_writeFds, NULL, NULL) < 0){
            perror("select fd probleme:");
            exit(EXIT_FAILURE);
        }
        for(int i = 0; i <= fd_max; i++){
            if (FD_ISSET(i, &_readFds)){ //one fd is ready to read
                it = master_servers.find(i);
                if (it != master_servers.end()){//master socket listner is ready to accept new connetion
                    newSocket = socketAccept(i);
                    FD_SET(newSocket, &_masterFds);
                    fd_max = newSocket > fd_max ? newSocket : fd_max;
                    // std::cout << "socket: " << newSocket << " accepted from " << i <<  "\n";
                    // int val = 1;
                    // setsockopt(newSocket, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));
                    _sockData.insert(std::pair<int, SocketData>(newSocket, SocketData())).first->second._server = it->second;
                } else { //we have some new connections .. lets get some data
                    // std::cout << "reading on: " << i << "\n";
                    dataHandler(i);
                }
            }
            else if (FD_ISSET(i, &_writeFds)){
                //std::cout << "sending on: " << i << "\n";
                sendResponse(i);
            }
        }
    }
}

// // void    MySocket::requestParssing(dataMap::iterator &it){
// //     std::cout << it->first << std::endl;
// //     std::cout << it->second._request << std::endl;
// //     std::cout << Red <<"------------\n" << Reset;
// //     //it->second._request.clear();
// //     //std::string respond = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 125\r\n\r\n<html> <head> <title>Page Title</title> </head> <body> <h1>This is a Heading</h1> <p>This is a paragraph.</p> </body> </html>\r\n";
// //     //std::cout << it->second._response;
// //     int size = send(it->first, &it->second._response[0], it->second._response.size(), 0);
// //     std::cout << size << " " << it->second._response.size();
// //     close(it->first);
// //     FD_CLR(it->first, &_masterFds);
// //     _sockData.erase(it);
// // }

void    MySocket::sendResponse(int newSocket){
    dataMap::iterator it = _sockData.find(newSocket);
    int check = send(it->first, &it->second._response[0], it->second._response.size(), 0);
    if (check < 0)
        throw std::string("Send ! error\n");
    it->second._dataWriteCheck += check;
    // std::cout << Green <<"--------------------------------/ sended response / --------------------------------\n" << Reset;
    // std::cout << "----\n" <<it->second._response << "----\n";
    // std::cout << check << "/" << it->second._response.size() << "\n";
    // std::cout << Green <<"--------------------------------/ end sended response / --------------------------------\n" << Reset;
    if (it->second._dataWriteCheck == it->second._response.size()){
        FD_CLR(it->first, &_writeFds);
        FD_CLR(it->first, &_masterFds);
        if (it->second._requestParssed.find("Connection")->second == "close"){ 
            close(it->first);
        }
        _sockData.erase(it->first);
    }
}

void     MySocket::dataHandler(const int &newSocket){
    int         byteRcived;
    char        buffer[1024] = {'\0'}; //data will be recived 1048576(max size recv buffer) byte by 1048576 byte
    // std::cout << newSocket << "------\n";
    byteRcived = recv(newSocket, &buffer[0], sizeof(buffer), 0);
    //std::string ss(buffer, 1024);
    //std::cout << byteRcived << ss.substr(0, 500) << "\n";
    if (byteRcived <= 0){
        if (byteRcived == 0){//EOS reached
            // std::cout << "connection closed on socket: " << newSocket << std::endl;
        } else {
            perror("Error reciving data from some new socket:");
        }
        FD_CLR(newSocket, &_readFds);
        FD_CLR(newSocket, &_writeFds);
        close(newSocket);
        FD_CLR(newSocket, &_masterFds);
        _sockData.erase(newSocket);
    } else {
        //i use [] map overload because it return ref to the string that contain request
        //if the socket associate with not exist it will be creat new one and return ref to it's string (request) for appending 
        dataMap::iterator it = _sockData.find(newSocket);
        it->second.append(buffer, byteRcived);
        if (it->second._dataReadCheck){
            //std::cout << "done reading\n";
            FD_CLR(it->first, &_readFds);
            FD_SET(it->first, &_writeFds);
            it->second.parse();//need implementation
            
            it->second.buildResponse(_config, _tools, it->second._server);//replace the object with only the map or vect that hold info from config file
            //std::cout << it->second._response;
            //sendResponse(it->);
            
            //requestParssing(it);//provisoire
        } else {
            //std::cout << "not yet\n";
        }
    }
    //return 0;
}