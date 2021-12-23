#include "ResponseBuilder.hpp"

// ResponseBuilder::ResponseBuilder(){
//     _response.clear();
// }

ResponseBuilder::ResponseBuilder(const reqMap &request, Config_parser config, const Tools &tools, int server): _request(request), _config(config), _tools(tools){
    _status_code = "200 OK";
    std::string host_uri = _request.find("Host")->second + _request.find("Uri")->second;
    //std::cout << host_uri << std::endl;
    _location = _config.get_location_info(server, host_uri);
    //std::cout << "locaaaaation "<<_location.root << std::endl;
    check_http(_request.find("Httpv")->second);
    build_response();
}

void    ResponseBuilder::check_http(const std::string &http){
    if (!(http.compare("HTTP/1.1"))){
        _response.append("HTTP/1.1 ");
        check_method(_request.find("Method")->second, _location.method);
    } else {
        _status_code = "505 HTTP Version Not Supported";
        // std::cout << http << ": " <<_status_code << std::endl;//don't delete or comment
        return;
    }
}

void    ResponseBuilder::check_method(const std::string &method, const std::string &allowed_methods){
    //std::cout << "checking allowed methods: " << method << "...\n";
    if (std::string("GET,POST,DELETE,PUT,TRACE,CONNECT,HEAD,OPTIONS").find(method) == std::string::npos){
        _status_code = "405 Method Not Allowed";
        // std::cout << method << ": " <<_status_code << std::endl;//don't delete or comment
        _status_code = "501 Internal Server Error";
        return;
    }
    else if(allowed_methods.find(method) != std::string::npos){
        check_uri(_request.find("Uri")->second, _location.root, _location.default_answer);
    } else {
            _status_code = "405 Method Not Allowed";
            // std::cout << method << ": " <<_status_code << std::endl;//don't delete or comment
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




    // for testing
    req_method = "";
    req_content_type = "";
    req_content_length = "";
    req_uri = "/1.php?userId=54545&b=value2";

    // req_method = "POST";
    // req_content_type = "application/x-www-form-urlencoded";
    // req_content_length = "10";

    // req_method = "GET";
    // req_content_type = "text/html";
    // req_content_length = "";

    // req_body = "v=900fs55";


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

    // reqMap::const_iterator it_uri = _request.find("Uri");
    // if (it_uri != _request.end()){
    //     req_uri = it_uri->second;
    // }



    Cgi_class cgi(_location, path, req_cookies, req_uri, req_method, req_content_type, req_content_length);
    cgi.send_cgi_body(req_body);

    // Read cgi response
    int nbytes;
	char cgi_buff[1024] = {0};
    while ((nbytes = read(cgi.pipe_fd[0], cgi_buff, 1024)) > 0) { 
           _body += cgi_buff;
    }
	close(cgi.pipe_fd[0]);
    return;
}

void    ResponseBuilder::check_uri(const std::string &uri, const std::string &root, const std::string &indexs){
    std::string path = root + uri;
    //std::cout << root << " + " << uri << "\n";
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) < 0){ // check if path exist both (reg or dir)
        _status_code = "404 Not Found";
        // std::cout << path << ": " <<_status_code << std::endl;//don't delete or comment
        return;
    } else { //the path exist but lets find if it a dir or reg
        //std::string file_path;
        if (S_ISDIR(path_stat.st_mode)){ // it's a dir -> seach for indexs
            if (uri[uri.size() - 1] != '/'){
                return redirection(uri);
            }
            if (check_indexs(path, indexs) != 0){
                return;
            }
        }
        std::string extention = std::string(path, path.rfind(".") + 1);
        if (extention == "php"){
            if (_location.cgi_extension == "php")
                check_cgi(path);
            else
                _status_code = "403 Forbidden";
            return;
        } else 
            check_content_type(extention);
        _response.append("Last-Modified: " + last_mod(path_stat.st_mtime) + "\r\n");
        stream_body(path);
    }
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
        // std::cout<<"List of Files & Folders:-\n";
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
            // std::cout<< d->d_name << s_file.st_size << last_mod(s_file.st_mtime) << std::endl;
        }
        _body += "</table></pre><hr></body></html>";
        closedir(dr);
    }
    else
        return -1;
    return 0;
}

int     ResponseBuilder::check_indexs(std::string &path, const std::string &indexs){
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
        if ((stat(path.c_str(), &path_stat)) < 0){//test path + index if exist .. if it is set path
            continue;
        }
        break;
    }
    if (i == strings.size()){ // the path + index not found
        if (_location.autoindex == "1"){
            check_autoindex(old_path);
            return 1;
        }
        else {
            _status_code = "404 Not Found";
            // std::cout << path << ": " <<_status_code << std::endl;//don't delete or comment
            return -1;
        }
    }
    return 0;
}

void    ResponseBuilder::redirection(const std::string &path){
    _status_code = "301 Moved Permanently";
    _red_location = path + "/";
    stream_body("./files/3xx.html");
}

void    ResponseBuilder::check_content_type(const std::string &extention){
    std::string content_type = _tools._mime.find(extention)->second;
    //it = _tools._mime.find(extention);
    if(!content_type.size()){
        _status_code = "415 Unsupported Media Type";
        return;
    } else {
        _status_code = "200 Ok";
        _response.append("Content-Type: " + content_type + "; charset=UTF-8\r\n");
    }
}

void    ResponseBuilder::stream_body(const std::string &path){
    std::ifstream input(path);
    std::ostringstream ss;
    ss << input.rdbuf();
    _body = ss.str();
    input.close();
}

std::string ResponseBuilder::last_mod(const time_t &lmod){
    struct tm *timeinfo;
    char buffer[80];

    timeinfo = localtime(&lmod);
    strftime(buffer,sizeof(buffer),"%a, %d %m %Y %H:%M:%S GMT",timeinfo);
    std::string str(buffer);
    //std::cout << "last mod" <<str << "\n";
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

    //std::cout << str;
    return str;
}

void    ResponseBuilder::build_response(){


    if(_location.cgi_path != "")
    {
        std::string cgiHeader;
        std::string cgiBody;
        size_t split;
        if ((split = _body.find("\r\n\r\n") + 4) < _body.size()){
            cgiHeader = _body.substr(0, split);
            cgiBody = _body.substr(split);
        }else{
            cgiHeader = _body.substr(0, _request.size() -  1);
        }

        // _body has headers
        _response.insert(9, _status_code + "\r\n");//insert after HTTP/1.1
        if (_status_code == "301 Moved Permanently")
            _response.append("Location: " + _red_location + "\r\n");
        _response.append("Date: " + get_time() + "\r\n");
        _response.append("Connection: keep-alive\r\n");
        _response.append("Server: Webserv1.3.3.7 \r\n");

        // Append cgi headers
        cgiHeader.erase(std::remove(cgiHeader.begin(), cgiHeader.end(), '\r'), cgiHeader.end());
        std::istringstream input(cgiHeader);
        std::string line, buffer;
        int i = 0;
        while(std::getline(input, line)){
            _response.append(line+"\r\n");
            i++;
        }

        _response.append("\r\n");
        _response.append(cgiBody + "\r\n");
        //std::cout << _response;
    }
    else
    {
        _response.insert(9, _status_code + "\r\n");//insert after HTTP/1.1
        if (_status_code == "301 Moved Permanently")
            _response.append("Location: " + _red_location + "\r\n");
        _response.append("Date: " + get_time() + "\r\n");
        _response.append("Connection: keep-alive\r\n");
        _response.append("Server: Webserv1.3.3.7 \r\n");
        _response.append("Content-Length: " + std::to_string(_body.size()) + "\r\n");
        _response.append("\r\n");
        _response.append(_body + "\r\n");
        //std::cout << _response;
    }
    
}