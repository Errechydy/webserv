#include "SocketData.hpp"

SocketData::SocketData(){
    _dataReadCheck = false;
    _recheckHeader = true;
    _recheckbody = true;
    _dataWriteCheck = 0;
    _reqHeaderLen = 0;
    _reqbodyLen = 0;
    //std::cout << "*******************constructor\n";
}

void SocketData::append(const char* buffer, int byteRecived){
    _request.append(buffer, byteRecived);
    checkDataReadEnd();
}

void SocketData::checkDataReadEnd(){
    size_t tmp = 0;
    size_t tmp2 = 0;

    if (_recheckHeader && (tmp2 = _request.find("\r\n\r\n")) != std::string::npos){
        //std::cout << "rnrn exist \n";
        _reqHeaderLen += tmp2 + 4;
        _recheckHeader = false;
    }

    if (_recheckbody && (((tmp = _request.find("Content-Length:")) != std::string::npos) || ((tmp = _request.find("content-length:")) != std::string::npos))){
        _reqbodyLen = std::stoi(_request.substr(tmp + 16, 100));//need more effort
        _recheckbody = false;
    }
    
    if ((tmp = _request.rfind("0\r\n\r\n")) != std::string::npos)
        _reqbodyLen = tmp + 5 - _reqHeaderLen;

    //std::cout << _request.size() << " = " << _reqHeaderLen << "+" << _reqbodyLen << "=" << _reqHeaderLen + _reqbodyLen << "\n";
    if (_request.size() == _reqHeaderLen + _reqbodyLen){
        //std::cout << _reqHeaderLen << "+" << _reqbodyLen << "=" << _reqHeaderLen + _reqbodyLen << "\n";
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
        std::cout << "chunked\n";
        while(true){
            tmp = _reqBody.find("\r\n", pos);
            //std::cout << _reqBody[tmp - 1];
            //std::cout << "-" <<_reqBody.substr(pos, tmp - pos) << "-\n";
            lenHex = _reqBody.substr(pos, tmp - pos);
            std::stringstream stream;
            stream << lenHex;
            stream >> std::hex >> lenDec;
            //std::cout << "lenHex: " << lenHex << "dec : " << lenDec << "\n";

            _reqBody.erase(pos, (tmp + 2) - pos);
            //newbody += _reqBody.substr(tmp + 2, lenDec);
            pos += lenDec;
            _reqBody.erase(pos, 2);
            stream.clear();
            if (lenDec == 0)
                break;
        }
        //std::cout << "**" << _reqBody << "**\n";
        
    } else {
        std::cout << "not chunked\n";            
    }

    // std::ofstream out("output.png");
    // out << _reqBody;
    // out.close();
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
            // std::cout << query << std::endl;
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
    std::cout << Red <<"--------------------------------/ request/ --------------------------------\n" << Reset;
    //std::cout << _request << "\n";
    std::cout << "---Headers---\n" << header << "-------\n";
    //std::cout << "---Body---\n" << _reqBody << "-------\n";
    std::cout << Red <<"--------------------------------/ end request/ --------------------------------\n" << Reset;
    
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
    check_query();
    check_chunked();
    _requestParssed.insert(reqPair("Body", _reqBody));
    // reqMap::iterator it;
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
