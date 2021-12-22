#include "SocketData.hpp"

SocketData::SocketData(){
    _dataReadCheck = false;
    _dataWriteCheck = 0;
    _request.clear();
    _response.clear();
    //std::cout << "*******************constructor\n";
}

void SocketData::append(const char* buffer, int byteRecived){
    _request.append(buffer, byteRecived);
    //checkDataReadEnd(std::string(buffer, byteRecived));
}

void SocketData::checkDataReadEnd(const std::string &dataChanked){
    size_t headerLen = 0;
    size_t bodyLen = 0;
    size_t tmp = 0;
    if ((headerLen = dataChanked.find("\r\n\r\n")) != std::string::npos){
        headerLen += 4;
        if ((tmp = _request.find("Content-Length:")) != std::string::npos){
            bodyLen = std::stoi(_request.substr(tmp + 16, 100));//need more effort
        } else 
            bodyLen = 0;
        if (_request.size() == headerLen + bodyLen){
            _dataReadCheck = true;
        }
    }
}

void SocketData::parse(){

    
    std::string header;
    size_t split;
    if ((split = _request.find("\r\n\r\n") + 4) < _request.size()){
        header = _request.substr(0, split);
        _reqBody = _request.substr(split);
    }else{
        header = _request.substr(0, _request.size() -  1);
    }

    // std::cout << Red <<"--------------------------------/ request/ --------------------------------\n" << Reset;
    // //std::cout << _request << "\n";
    // std::cout << "---Headers---\n" << header << "-------\n";
    // std::cout << "---Body---\n" << _reqBody << "-------\n";
    // std::cout << Red <<"--------------------------------/ end request/ --------------------------------\n" << Reset;
    
    header.erase(std::remove(header.begin(), header.end(), '\r'), header.end());//remove /r (printing problem)
    
    std::istringstream input(header);
    std::string line, buffer;
    int i = 0;
    while(std::getline(input, line)){//dilemiter \n by defult
        std::istringstream input2(line);
        //std::cout << line << "/n";
        if (i == 0){//specila parssing for the fisrt line split = ' '
            int j = 0;
            while (std::getline(input2, buffer, ' ')){
                if (j == 0)
                    _requestParssed.insert(reqPair("Method", buffer));
                else if (j == 1){
                    _requestParssed.insert(reqPair("Uri", buffer));
                }
                else if (j == 2)
                    _requestParssed.insert(reqPair("Httpv", buffer));
                j++;
            }
        } else if (line.find(":") != std::string::npos){
            std::getline(input2, buffer, ':');//use getline one time because there is more value with :
            //if (buffer.size() > 1)//don't insert empty line
                _requestParssed.insert(reqPair(buffer, std::string(line, buffer.size() + 2)));//add 2 to remove ': '
        }
        //std::cout << "here\n";
        buffer.clear();
        input2.clear();
        i++;
    }

    _requestParssed.insert(reqPair("Body", _reqBody));
    reqMap::iterator it;
    // std::cout << Red <<"--------------------------------/ request parssed / --------------------------------\n" << Reset;
    // for (it = _requestParssed.begin(); it != _requestParssed.end(); it++){
    //     std::cout << "** -" << it->first << "-:-" << it->second << "-\n";
    // }
    // std::cout << Red <<"--------------------------------/ end request parssed / --------------------------------\n"<< Reset;
}

void    SocketData::buildResponse(const Config_parser &config, const Tools &tools, int server){
    //std::cout << "heeere\n";
    ResponseBuilder response(_requestParssed, config, tools, server);
    _response = response._response;
    //std::cout << _response;
}
