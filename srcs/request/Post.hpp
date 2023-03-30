#ifndef POST_HPP
#define POST_HPP

#include "../headers_cpp.hpp"
class Server;
class Client;
class Post
{
    private:

        int         body_or_head;
        bool        is_created;
    public:

        int             _post_type;
        unsigned int    _chunk_len;
        char            _hex[20];
        size_t          _hex_len;
        bool            _hex_ready;
        std::string boundary;

        Post();
        ~Post();
        
        void        create_file(Server &, Client *);
        void        call_post_func(Server &server, Client *client);
        void        normal_post(Server &server, Client *client);
        void        chunked_post(Server &server, Client *client);
        bool        check_hex();
        int         Treat_Post(Client *ctl, Server &serv);
        // void        exec_head(std::string buff, Server &serv, std::string &path);
        // void        exec_body(std::string buff, Server &serv, std::string &path);
        // int         skip_hex(std::string body);
        // int         hexToDec(const std::string& hexStr);
        std::string check_hexa(std::string buff);
};
#endif