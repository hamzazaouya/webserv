#include "server.hpp"
#include "../parsing/location.hpp"
#include "../parsing/parce_server.hpp"
#include "socket.hpp"
#include "client.hpp"

Server::Server(parce_server &server_data, std::map<std::string, std::string> &file_extensions)
{
    this->_port = server_data.port;
    this->_host_name = server_data.host_name;
    this->_max_client_body_size = server_data.max_client_body_size;
    this->_error_page = server_data.error_page;
    this->_locations = server_data.locations;
    Socket socket(this->_port);
    this->_server_socket = socket.get_socket();
    this->file_extensions = file_extensions;
    std::map<std::string, std::string>::iterator iter;
}

std::list<location> Server::get_locations() const
{
    return(this->_locations);
};

void  Server::init_sockfds()
{
    FD_ZERO(&this->_reads);
    FD_ZERO(&this->_writes);
    FD_SET(this->_server_socket, &this->_reads);
    this->_max_socket = this->_server_socket;
    std::list<Client *>::iterator iter;
    for(iter = this->_clients.begin(); iter != this->_clients.end(); iter++)    {
        FD_SET((*iter)->get_sockfd(), &this->_writes);
        FD_SET((*iter)->get_sockfd(), &this->_reads);
        if ((*iter)->get_sockfd() > this->_max_socket)
            this->_max_socket = (*iter)->get_sockfd();
    }
}

void    Server::wait_on_clients()
{
    struct timeval restrict;

    this->init_sockfds();
    restrict.tv_sec = 0;
    restrict.tv_usec = 0;
    if (select(1000, &(this->_reads), &(this->_writes), NULL, &restrict) < 0)
    {
        std::cerr << "select() failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

const char *get_client_address(Client *ci)
{
    static char address_buffer[100];
    getnameinfo((struct sockaddr*)&ci->_address,
    ci->_address_length,
    address_buffer, sizeof(address_buffer), 0, 0,
    NI_NUMERICHOST);
    return address_buffer;
}

void    Server::accept_new_client()
{
    Client *client = new Client;

    SOCKET r = accept(this->_server_socket, \
        reinterpret_cast<struct sockaddr *>(&client->_address), \
        &(client->_address_length));
    client->set_sockfd(r);
    if (!ISVALIDSOCKET(client->get_sockfd()))
    {
        std::cerr << "accept() failed." << std::endl;
        exit(EXIT_FAILURE);
    }
    this->_clients.push_back(client);
    this->_max_socket = std::max(_max_socket, r);
}

void    Server::run_serve()
{
    this->wait_on_clients();
    if (FD_ISSET(this->_server_socket, &this->_reads))
        this->accept_new_client();
    else
        this->serve_clients();
}

void    Server::serve_clients()
{
    std::list<Client *>::iterator   iter;
    for(iter = this->_clients.begin(); iter != this->_clients.end(); iter++)
    {
        // std::cout << "client number" << (*iter)->get_sockfd() << std::endl;
        if(FD_ISSET((*iter)->get_sockfd(), &this->_reads) && !(*iter)->_is_ready)
        {
            memset(this->_request, 0, MAX_REQUEST_SIZE + 1);
            this->_request_size = recv((*iter)->get_sockfd(), this->_request, MAX_REQUEST_SIZE, 0);
    
            if (this->_request_size < 1)
            {
                std::cerr << "Unexpected disconnect from << " << get_client_address(*iter) << std::endl;
                drop_client(iter);
                continue ;
            }
            (*iter)->set_received_data(this->_request_size);
            if(!(*iter)->_request_type)
            {
                if(memmem(_request, _request_size, "\r\n\r\n", 4))
                {
                    // std::cout<<_request<<std::endl;
                    (*iter)->error_pages = this->_error_page;
                    Request req(_request, iter);
                    Check_path path(iter, *this);
                    if (path.skip == 1)
                    {
                        (*iter)->_is_ready = 1;
                        break ;
                    }
                    else
                    {
                        if(req.method == "POST")
                        {
                            (*iter)->init_post_data();
                            (*iter)->_request_type = true;
                            if ((*iter)->post.check_post((*iter)) == 1)
                            {
                                this->seperate_header(*iter);
                                (*iter)->post.call_post_func(*this, (*iter));
                            }
                            else
                                (*iter)->post.Treat_Post((*iter), *this);
                        }
                        else if (req.method == "DELETE")
                            (*iter)->del.erase((*iter), *this);
                        else if(req.method == "GET")
                        {
                            (*iter)->get.get_requested_resource(iter);
                            if ((*iter)->get.state == 2)
                                (*iter)->_is_ready = 2;
                            else
                                (*iter)->_is_ready = 1;
                        }
                    }
                }
                // else
                //     std::cout << "Your header is large" << std::endl;
                // std::cout << "client_size : "<< this->_clients.size()<< std::endl;
                // std::cout<<"hello_post"<<std::endl;
            }
            else
            { // this else is for just post becouse post containe the body.
                (*iter)->post.call_post_func(*this, *iter);
            }
        }
        else if(FD_ISSET((*iter)->get_sockfd(), &this->_writes) && (*iter)->_is_ready)
        {
            if ((*iter)->header == 0)
            {
                this->respons(iter);
                // std::cout<< "header ====== : " << (*iter)->resp<<std::endl;
                write ((*iter)->get_sockfd(), (*iter)->resp.c_str(), (*iter)->resp.length());
                (*iter)->header = 1;
            }
            else
            {
                std::string str;
                char buffer[MAX_REQUEST_SIZE + 1];
                memset(buffer, 0 , MAX_REQUEST_SIZE + 1);
                if ((*iter)->file_is_open == 0)
                {
                    // std::cout<<"filePath : "<< (*iter)->loc_path << std::endl;
                    (*iter)->filein.open((*iter)->loc_path, std::ios::binary);
                    (*iter)->file_is_open = 1;
                }
                (*iter)->filein.read(buffer, MAX_REQUEST_SIZE);
                int szReaded = (*iter)->filein.gcount();
                // std::cout<<"reading ..."<<std::endl;
                // std::cout << "size readed =  " <<  szReaded  << " buffer ==== " << buffer<< std::endl;
                if (szReaded <= 0)
                {
                    std::cout<<"finish reading ..."<<std::endl;
                    (*iter)->filein.close();
                    // std::cout<<"finishing"<<std::endl;
                    drop_client(iter);
                    if (this->_clients.size() == 0)
                        break ;
                }

                // std::cout<<SIGPIPE<<std::endl;
                // std::cout<<buffer<<std::endl;
                write ((*iter)->get_sockfd(), buffer, szReaded);
                // std::cout << "Hello world from ready to write" << std::endl;
            }
            // else if((*iter)->_is_ready == 2)
            // {
            //     std::string str;
            //     char buffer[MAX_REQUEST_SIZE + 1];
            //     memset(buffer, 0 , MAX_REQUEST_SIZE + 1);
            //     if ((*iter)->file_is_open == 0)
            //     {
            //         (*iter)->buffer = (*iter)->list_files;
            //         (*iter)->file_is_open = 1;
            //     }
            //     (*iter)->buffer.read(buffer, MAX_REQUEST_SIZE);
            // }
        }
    }
                // std::cout << "hello from outside" << std::endl;
}

void    Server::drop_client(std::list<Client *>::iterator client)
{
    CLOSESOCKET((*client)->get_sockfd());
    std::list<Client *>::iterator iter;

    for(iter = this->_clients.begin(); iter != this->_clients.end(); iter++)
    {
        if((*client)->get_sockfd() == (*iter)->get_sockfd())
            iter = this->_clients.erase(iter);
        return ;
    }
    std::cerr << "Drop Client not found !" << std::endl;
}

void Server::seperate_header(Client *client)
{
    int x = 4;
    int pos = (char *) memmem(_request, _request_size, "\r\n\r\n", 4) - _request;

    if(client->post._post_type == 2)
        x = 2;
    this->_request_size -= (pos + x);
    for(int i = (pos + x); i < MAX_REQUEST_SIZE; i++)
        _request[i - (pos + x)] = _request[i];
    memset(_request + (MAX_REQUEST_SIZE - (pos + x)), 0, x);
}

void    Server::respons(std::list<Client *>::iterator iter)
{
    (*iter)->resp.append((*iter)->http);
    (*iter)->resp.append(" ");
    (*iter)->resp.append(std::to_string((*iter)->status_code));
    (*iter)->resp.append(" ");
    (*iter)->resp.append((*iter)->status);
    (*iter)->resp.append("\r\n");
    if ((*iter)->status_code == 301)
    {
        (*iter)->resp.append("Location: ");
        (*iter)->resp.append((*iter)->redirect_301);
        (*iter)->resp.append("\r\n\r\n");
        return ;
    }
    (*iter)->resp +="Content-Type: ";
    (*iter)->generate_extensions_2();
    std::string type = ft_get_extention(&(*iter)->loc_path[(*iter)->loc_path.rfind('.')], iter);
    (*iter)->resp += type;
    (*iter)->resp += "\r\n";
    (*iter)->resp.append("Content-Length: ");
    std::ifstream filein;
    filein.open((*iter)->loc_path,std::ios::binary);
    filein.seekg(0, std::ios::end);
    size_t file_size = filein.tellg();
    filein.close();
    (*iter)->resp.append(std::to_string(file_size));
    (*iter)->resp.append("\r\n\r\n");
}

std::string Server::ft_get_extention(std::string str, std::list<Client *>::iterator iter)
{
    std::map<std::string,   std::string>::iterator it = (*iter)->file_extensions_get.find(str);
    if (it != (*iter)->file_extensions_get.end())
        return (it->second);
    else
        return str;
}

Server::~Server() {}
