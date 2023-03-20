#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../headers_cpp.hpp"
class Server;
class location;
class Client;
class Request
{
    private:
        void    fill_map_request(std::string   &token);
        std::string from_hexa_to_decimal(std::string &str);
        int get_decimal(std::string tmp);
    public:
        std::string method;
        std::string path;
        std::string query;
        std::string http;
        std::map<std::string, std::vector<std::string> > request;
        Request();
        Request(std::string &buffer, std::list<Client *>::iterator   iter);
};
#endif