#ifndef POST_HPP
#define POST_HPP

#include "../headers_cpp.hpp"
class Server;
class Client;
class Post
{
    private:

        int         body_or_head;
        
    public:

        int         _post_type;
        std::string boundary;

        Post();
        ~Post();

        void        call_post_func(Server &server, Client *client);
        void        normal_post(Server &server, Client *client);
        void        exec_head(std::string buff, Server &serv, std::string &path);
        void        exec_body(std::string buff, Server &serv, std::string &path);
        int         skip_hex(std::string body);
        int         hexToDec(const std::string& hexStr);
        int         Treat_Post(Client *ctl, Server &serv);
        std::string check_hexa(std::string buff);
};
#endif