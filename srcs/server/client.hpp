#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../headers_cpp.hpp"

#define SOCKET  int
#define DATA    int

#include "../request/Post.hpp"
typedef struct sockaddr_storage sock_storage;

class Client
{
    private:
        SOCKET  _sockfd;
        DATA    _received_data;
        
    public:
        std::map<std::string, std::vector<std::string> >  request_pack;
        //std::vector<std::string> method
        std::string     method;
        int             content_type;
        std::string     boundary;
        std::string     path;
        std::string     query;
        std::string     http;
        Post            post;
        socklen_t       _address_length;
        sock_storage    _address;
        std::string     _request;
        int             _request_type;
        Client();
        Client(const Client& rhs);
        void            init_post_data();
        Client          &operator=(const Client& rhs);
        SOCKET          get_sockfd(void);
        void            set_sockfd(SOCKET sfd);
        void            set_received_data(DATA data);
        DATA            get_received_data(void);
        ~Client();
};

#endif