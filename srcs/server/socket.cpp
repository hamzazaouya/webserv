#include "socket.hpp"

Socket::Socket(int  port): _port(port)
{
    this->creat_socket();
}

Socket::Socket(const Socket &rhs)
{
    this->_sockfd = rhs._sockfd;
}

Socket& Socket::operator = (const Socket& rhs)
{
    this->_sockfd = rhs._sockfd;
    return (*this);
}

void    Socket::init_socket()
{
    this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (!ISVALIDSOCKET(this->_sockfd))
    {
        std::cerr << "socket() failed." << GETSOCKETERRNO() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void    Socket::bind_socket()
{
    struct sockaddr_in addr;
    int use = 1;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(this->_port);
    setsockopt(_sockfd,SOL_SOCKET,SO_REUSEADDR, &use,sizeof(use));
    if(bind(this->_sockfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)))
    {
        std::cerr << "bind() failed." << GETSOCKETERRNO() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void    Socket::listen_socket()
{
    if(listen(this->_sockfd, 10))
    {
        std::cerr << "listen() failed." << GETSOCKETERRNO() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void  Socket::creat_socket()
{
    this->init_socket();
    this->bind_socket();
    this->listen_socket();
}

SOCKET  Socket::get_socket(void) const
{
    return (this->_sockfd);
}

Socket::~Socket(){}