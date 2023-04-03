#ifndef SERVER_HPP
#define SERVER_HPP

#include "../headers_cpp.hpp"
#include "../request/request.hpp"
#include "../request/check_path.hpp"

class Client;
class Socket;
class parce_server;
class location;

#define SOCKET              int
#define PORT                int
#define MAX_REQUEST_SIZE    1024

class Server
{
    private:

        PORT                        _port;
        SOCKET                      _server_socket;
        SOCKET                      _max_socket;
        fd_set                      _writes;
        std::string                 _host_name;
        unsigned int                _max_client_body_size;
        std::list<location>         _locations;
        std::list<Client *>         _clients;
        std::vector<std::string>    _error_page;

        void    init_sockfds();
        void    wait_on_clients();
        void    new_connection();
        void    accept_new_client();
        void    drop_client(std::list<Client *>::iterator client);
        void    serve_clients();
        void    seperate_header(Client *client);

    public:
        int                     _request_size;
        char                    _request[MAX_REQUEST_SIZE + 1];
        fd_set                  _reads;
        std::list<location>     get_locations() const;
        int get_max_client_body_size() const {return (_max_client_body_size);}
        std::string             ft_get_extention(std::string str, std::list<Client *>::iterator iter);
        std::map<std::string,   std::string>    file_extensions ;
        void    respons(std::list<Client *>::iterator iter);

        Server();
        ~Server();
        Server(parce_server &server_data, std::map<std::string, std::string> &file_extensions);

        void    run_serve();
        // const char *get_client_address(Client *);
        // void    serve_resource(Client client, const char *path);
};

#endif
