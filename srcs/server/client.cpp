#include "client.hpp"

Client::Client(): _received_data(0)
{
    this->_address_length = sizeof(this->_address);
    this->_request_type = false;
    content_type = 0;
}

Client::Client(const Client& rhs)
{
    this->_sockfd = rhs._sockfd;
    this->_received_data = rhs._received_data;
    this->_address = rhs._address;
    this->_address_length = rhs._address_length;
    this->content_type = rhs.content_type;
}

Client &Client::operator=(const Client& rhs)
{
    this->_sockfd = rhs._sockfd;
    this->_received_data = rhs._received_data;
    this->_address = rhs._address;
    this->_address_length = rhs._address_length;
    this->content_type = rhs.content_type;
    return (*this);
}

SOCKET  Client::get_sockfd(void)
{;
    return (this->_sockfd);
}

void    Client::set_sockfd(SOCKET sfd)
{
    this->_sockfd = sfd;
}

DATA    Client::get_received_data(void)
{
    return (this->_received_data);
}

void    Client::set_received_data(DATA data)
{
    this->_received_data += data;
}

void    Client::init_post_data()
{
    this->post.boundary = this->boundary;
    this->post._post_type = this->content_type;
}

void    Client::generate_file_name( std::string &mime_type, std::map<std::string,\
                                    std::string> &file_extensions)
{
    std::map <std::string, std::string>::iterator iter;
    time_t      now;

    now = std::time(0);
    this->file_path.push_back('/');
    this->file_path.append(std::to_string(now));
    this->file_path.append("_");
    this->file_path.append(std::to_string(id++));
    iter = file_extensions.find(mime_type);
    if(iter != file_extensions.end())
        this->file_path.append((*iter).second);
    std::cout << file_path << std::endl;
}

Client::~Client() {}