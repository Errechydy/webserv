#include "SocketData.hpp"

SocketData::SocketData(){
    _dataReadCheck = false;
    _recheckHeader = true;
    _recheckbody = true;
    _dataWriteCheck = 0;
    _reqHeaderLen = 0;
    _reqbodyLen = 0;
}

void SocketData::append(const char* buffer, int byteRecived){
    _request.append(buffer, byteRecived);
    checkDataReadEnd();
}

void SocketData::checkDataReadEnd(){
    size_t tmp = 0;
    size_t tmp2 = 0;

    if (_recheckHeader && (tmp2 = _request.find("\r\n\r\n")) != std::string::npos){
        _reqHeaderLen += tmp2 + 4;
        _recheckHeader = false;
    }

    if (_recheckbody && (((tmp = _request.find("Content-Length:")) != std::string::npos) || ((tmp = _request.find("content-length:")) != std::string::npos))){
        _reqbodyLen = std::stoi(_request.substr(tmp + 16, 100));
        _recheckbody = false;
    }
    
    if ((tmp = _request.rfind("0\r\n\r\n")) != std::string::npos)
        _reqbodyLen = tmp + 5 - _reqHeaderLen;

    if (_request.size() == _reqHeaderLen + _reqbodyLen){
        _dataReadCheck = true;
    }
}

void SocketData::check_chunked(){
    reqMap::iterator it;
    size_t pos = 0;
    std::string lenHex;
    size_t lenDec = 0;
    size_t tmp;
    std::string newbody;

    it = _requestParssed.find("Transfer-Encoding");
    if (it == _requestParssed.end())
        it = _requestParssed.find("transfer-encoding");
    if (it != _requestParssed.end() && it->second == "chunked"){
        while(true){
            tmp = _reqBody.find("\r\n", pos);
            lenHex = _reqBody.substr(pos, tmp - pos);
            std::stringstream stream;
            stream << lenHex;
            stream >> std::hex >> lenDec;
            _reqBody.erase(pos, (tmp + 2) - pos);
            pos += lenDec;
            _reqBody.erase(pos, 2);
            stream.clear();
            if (lenDec == 0)
                break;
        }        
    }
}

void SocketData::check_query(){
    std::string query;
    reqMap::iterator it = _requestParssed.find("Uri");

    if (it != _requestParssed.end()){
        size_t pos = it->second.find("?");
        if (pos != std::string::npos){
            query = std::string(it->second, it->second.find("?"));
            it->second.erase(pos);
            _requestParssed.insert(reqPair("Query", query));
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
    header.erase(std::remove(header.begin(), header.end(), '\r'), header.end());
    
    std::istringstream input(header);
    std::string line, buffer;
    int i = 0;
    while(std::getline(input, line)){
        std::istringstream input2(line);
        if (i == 0){
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
            std::getline(input2, buffer, ':');
            _requestParssed.insert(reqPair(buffer, std::string(line, buffer.size() + 2)));
        }
        buffer.clear();
        input2.clear();
        i++;
    }
    check_query();
    check_chunked();
    _requestParssed.insert(reqPair("Body", _reqBody));
}

void    SocketData::buildResponse(const Config_parser &config, const Tools &tools, int server){
    ResponseBuilder response(_requestParssed, config, tools, server);
    _response = response._response;
}
