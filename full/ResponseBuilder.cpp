#include "ResponseBuilder.hpp"


ResponseBuilder::ResponseBuilder(const reqMap &request, Config_parser config, const Tools &tools, int server): _request(request), _config(config), _tools(tools){
    _status_code = "200 Ok";
    if (_request.find("Host") == _request.end()){
        _status_code = "400 Bad Request";
        _response.append("HTTP/1.1 ");
        build_response();
        return;
    }
    std::string host_uri = _request.find("Host")->second + _request.find("Uri")->second;
    _location = _config.get_location_info(server, host_uri);
    check_http(_request.find("Httpv")->second);
    build_response();
}

void    ResponseBuilder::check_http(const std::string &http){
    if (!(http.compare("HTTP/1.1"))){
        _response.append("HTTP/1.1 ");
        check_method(_request.find("Method")->second, _location.method);
    } else {
        _status_code = "505 HTTP Version Not Supported";
        return;
    }
}

void    ResponseBuilder::check_method(const std::string &method, const std::string &allowed_methods){
    if (std::string("GET,POST,DELETE,PUT,TRACE,CONNECT,HEAD,OPTIONS").find(method) == std::string::npos){
        _status_code = "501 Not Implemented";
        return;
    }
    else if(allowed_methods.find(method) != std::string::npos){
        check_uri(_request.find("Uri")->second, _location.root, _location.default_answer);
    } else {
            _status_code = "405 Method Not Allowed";
            return;
    }
}

void    ResponseBuilder::check_cgi(const std::string &path){
    std::string req_uri = "";
    std::string req_cookies = "";
    std::string req_body = "";
    std::string req_method = "";
    std::string req_content_type = "";
    std::string req_content_length = "NULL";
    req_method = "";
    req_content_type = "";
    req_content_length = "";
    req_uri = "";

    reqMap::const_iterator it_content_type = _request.find("content-type");
    if (it_content_type != _request.end()){
        req_content_type = it_content_type->second;
    }
    reqMap::const_iterator it_content_length = _request.find("content-length");
    if (it_content_length != _request.end()){
        req_content_length = it_content_length->second;
    }
    reqMap::const_iterator it_method = _request.find("Method");
    if (it_method != _request.end()){
        req_method = it_method->second;
    }
    reqMap::const_iterator it_cookie = _request.find("Cookie");
    if (it_cookie != _request.end()){
        req_cookies = it_cookie->second;
    }
    reqMap::const_iterator it_body = _request.find("Body");
    if (it_body != _request.end()){
        req_body = it_body->second;
    }

    reqMap::const_iterator it_uri = _request.find("Query");
    if (it_uri != _request.end()){
        req_uri = it_uri->second;
    }
    Cgi_class cgi(_location, path, req_cookies, req_uri, req_method, req_content_type, req_content_length);
    cgi.send_cgi_body(req_body);
    int nbytes;
	char cgi_buff[1024] = {0};
    while ((nbytes = read(cgi.pipe_fd[0], cgi_buff, 1024)) > 0) { 
           _body += cgi_buff;
    }
	close(cgi.pipe_fd[0]);
    return;
}

void    ResponseBuilder::check_uri(const std::string &uri, const std::string &root, const std::string &indexs){
    if (uri.size() > 1024){
        _status_code = "414 URI Too Long";
        return;
    }
    std::string path = root + uri;
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) < 0){
        _status_code = "404 Not Found";
        return;
    } else {
        if (_location.redirect.size() > 0){
            std::string str;
            if(_location.redirect.begin()->first == "300")
                str = "300 Multiple Choices";
            else if(_location.redirect.begin()->first == "301")
                str = "301 Moved Permanently";
            else if(_location.redirect.begin()->first == "302")
                str = "302 Found";
            else if(_location.redirect.begin()->first == "303")
                str = "303 See Other";
            else if(_location.redirect.begin()->first == "304")
                str = "304 Not Modified";
            else if(_location.redirect.begin()->first == "307")
                str = "307 Temporary Redirect";
            else if(_location.redirect.begin()->first == "308")
                str = "308 Permanent Redirect";
            else
                str = "301 Moved Permanently";
            _status_code = str;
            _red_location = _location.redirect.begin()->second;
            return;
        }
        if (S_ISDIR(path_stat.st_mode)){
            if (uri[uri.size() - 1] != '/'){
                return redirection(uri);
            }
            if (check_indexs(path, indexs) != 0){
                return;
            }
        }
        if (access(path.c_str(), R_OK) != 0){
            _status_code = "403 Forbidden";
            return;
        }
        std::string extention = std::string(path, path.rfind(".") + 1);
        if (_location.cgi_extension != "" && _location.cgi_path != "") {
            if (extention == _location.cgi_extension){
                if (access(_location.cgi_path.c_str(), F_OK) != 0){
                    _status_code = "502 Bad Gateway";
                    return;
                }
                check_cgi(path);
            }
            else
                _status_code = "403 Forbidden";
            return;
        } else 
            check_content_type(extention);
        if (check_max_body() < 0)
            return;
        if(_request.find("Method")->second == "POST"){
            if (check_uploads() < 0)
                return;
        }
        _response.append("Last-Modified: " + last_mod(path_stat.st_mtime) + "\r\n");
        stream_body(_body ,path);
    }
}

int     ResponseBuilder::check_max_body(){
    size_t maxBodySize = std::stoi(_location.client_max_body_size) * 1024 * 1024;
    if (_request.find("Body")->second.size() > maxBodySize){
        _status_code = "413 Payload Too Large";
        return -1;
    }
    return 0;
}

int     ResponseBuilder::check_uploads(){
    std::string content_type;
    std::string boundry;
    std::string filename;

    reqMap::const_iterator it;
    if (((it = _request.find("Content-Type")) != _request.end()) || ((it = _request.find("content-type")) != _request.end())){
        content_type = it->second;
        if (content_type.find("multipart/form-data") != std::string::npos){
            size_t pos = content_type.find("boundary=") + 9;
            boundry = content_type.substr(pos);
            it = _request.find("Body");
            if ((pos = it->second.find("filename=\"")) != std::string::npos){
                if (_location.accept_upload == "0"){
                    _status_code = "403 Forbidden";
                    return -1;
                }
                int pos2 = it->second.find("\"", pos + 10);
                filename = it->second.substr(pos + 10, pos2 - (pos + 10));

                pos2 = it->second.find("\r\n\r\n") + 4;
                pos = it->second.find(boundry + "--");
                std::ofstream out(_location.root + _location.upload_path + "/" + filename);
                out << it->second.substr(pos2, pos - pos2 - 2);
                out.close();
            }
        }
    }

    return 0;
}

int    ResponseBuilder::check_autoindex(const std::string &path){
    struct dirent *d;
    struct stat s_file;
    DIR *dr;
    dr = opendir(path.c_str());

    if(dr != NULL)
    {
        _response.append("Content-Type: text/html; charset=UTF-8\r\n");
        _body += "<html><head><title>Index of " + _request.find("Uri")->second + "</title></head><body><h1>Index of " +  _request.find("Uri")->second + "</h1><hr><pre><table style=\"width:70%/;text-align: left;\">";
        for(d=readdir(dr); d!=NULL; d=readdir(dr))
        {
            if (stat((path + d->d_name).c_str(), &s_file) < 0)
                return -1;
            _body += "<tr><th style=\"padding-right: 250px;\"><a href=\"$X\">$Y</a></th>";
            _body.replace(_body.find("$X"), 2, d->d_name);
            _body.replace(_body.find("$Y"), 2, d->d_name);
            _body += "<th style=\"padding-right: 250px;\"><p>" + last_mod(s_file.st_mtime) + "</p></th>";
            if(S_ISREG(s_file.st_mode))
                _body += "<th style=\"padding-right: 250px;\"><p>" + std::to_string(s_file.st_size) + "</p></th>";
            else
                _body += "<th style=\"padding-right: 250px;\"><p>-</p></th>";
            _body += "</tr>";
        }
        _body += "</table></pre><hr></body></html>";
        closedir(dr);
    }
    else
        return -1;
    return 0;
}

int     ResponseBuilder::check_indexs(std::string &path, const std::string &indexs){

    if (indexs.size() == 0){
        _status_code = "403 Forbidden";
        return -1;
    }

    struct stat path_stat;
    std::string old_path = path;
    std::vector<std::string> strings;
    std::istringstream file(indexs);
    std::string s;
    while (std::getline(file, s, ' ')){
        strings.push_back(s);
    }
    s.clear();
    size_t i = 0;
    for (i = 0; i < strings.size(); i++){
        path = old_path + strings[i];
        if ((stat(path.c_str(), &path_stat)) < 0){
            continue;
        }
        break;
    }
    if (i == strings.size()){
        if (_location.autoindex == "1"){
            check_autoindex(old_path);
            return 1;
        }
        else {
            _status_code = "404 Not Found";
            return -1;
        }
    }
    return 0;
}

void    ResponseBuilder::redirection(const std::string &path){
    _status_code = "301 Moved Permanently";
    _red_location = path + "/";
}

void    ResponseBuilder::check_content_type(const std::string &extention){
    std::string content_type = _tools._mime.find(extention)->second;
    if(!content_type.size()){
        _status_code = "415 Unsupported Media Type";
        return;
    } else {
        _status_code = "200 Ok";
        _response.append("Content-Type: " + content_type + "; charset=UTF-8\r\n");
    }
}

void    ResponseBuilder::stream_body(std::string &body, const std::string &path){
    std::ifstream input(path);
    std::ostringstream ss;
    ss << input.rdbuf();
    body = ss.str();
    input.close();
}

std::string ResponseBuilder::last_mod(const time_t &lmod){
    struct tm *timeinfo;
    char buffer[80];

    timeinfo = localtime(&lmod);
    strftime(buffer,sizeof(buffer),"%a, %d %m %Y %H:%M:%S GMT",timeinfo);
    std::string str(buffer);
    return str;
}

std::string ResponseBuilder::get_time(){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%a, %d %m %Y %H:%M:%S GMT",timeinfo);
    std::string str(buffer);
    return str;
}

void    ResponseBuilder::build_response(){
    if(_location.cgi_path != "")
    {
        std::string cgiHeader;
        std::string cgiBody;
        size_t split;
        bool header_exist = false;

        if(_body.find("\r\n\r\n") != std::string::npos)
            header_exist = true;
        if(header_exist)
        {
            split = _body.find("\r\n\r\n");
            cgiHeader = _body.substr(0, split);
            cgiBody = _body.substr(split);
        }
        else 
            cgiBody = _body;
        _response.insert(9, _status_code + "\r\n");
        if (_status_code.compare(0, 3, "200") != 0){
            _response.append("Content-Type: text/html; charset=UTF-8\r\n");
            std::map<std::string, std::string>::iterator it = _location.error_page.find(std::string(_status_code, 0, 3));
            if (it != _location.error_page.end()){
                stream_body(cgiBody, _location.root + it->second);
            } else{
                stream_body(cgiBody, "./files/xxx.html");
                size_t pos;
                while ((pos = cgiBody.find("@XXX@")) != std::string::npos)
                {
                    cgiBody.replace(pos, 5, _status_code);
                }
            }
        }
        if (_status_code == "301 Moved Permanently")
            _response.append("Location: " + _red_location + "\r\n");
        _response.append("Date: " + get_time() + "\r\n");
        _response.append("Connection: keep-alive\r\n");
        _response.append("Server: Webserv1.3.3.7 \r\n");
        _response.append("Content-Length: " + std::to_string(cgiBody.size()) + "\r\n");
        if(header_exist)
        {
            cgiHeader.erase(std::remove(cgiHeader.begin(), cgiHeader.end(), '\r'), cgiHeader.end());
            std::istringstream input(cgiHeader);
            std::string line, buffer;
            int i = 0;
            while(std::getline(input, line)){
                _response.append(line+"\r\n");
                i++;
            }
        }
        _response.append("\r\n");
        _response.append(cgiBody + "\r\n");
    } else {
        _response.insert(9, _status_code + "\r\n");
        if (_status_code.compare(0, 3, "200") != 0){
            _response.append("Content-Type: text/html; charset=UTF-8\r\n");
            std::map<std::string, std::string>::iterator it = _location.error_page.find(std::string(_status_code, 0, 3));
            if (it != _location.error_page.end()){
                stream_body(_body, _location.root + it->second);
            } else{
                stream_body(_body, "./files/xxx.html");
                size_t pos;
                while ((pos = _body.find("@XXX@")) != std::string::npos)
                {
                    _body.replace(pos, 5, _status_code);
                }
            }
        }
        if (_status_code.compare(0, 1, "3") == 0)
            _response.append("Location: " + _red_location + "\r\n");
        _response.append("Date: " + get_time() + "\r\n");
        _response.append("Connection: keep-alive\r\n");
        _response.append("Server: Webserv1.3.3.7 \r\n");
        _response.append("Content-Length: " + std::to_string(_body.size()) + "\r\n");
        _response.append("\r\n");
        _response.append(_body + "\r\n");
    }

}