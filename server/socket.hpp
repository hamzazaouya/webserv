#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#define EXIT_FAILURE    1
#define EXIT_SUCCESS    0
#define STD_ERROR       2

class Socket
{
    private:
        SOCKET      _sockfd;
        int         _port;
    public:
        Socket(int  port);
        Socket(const Socket &rhs);
        Socket  &operator = (const Socket& rhs);
        void    init_socket();
        void    bind_socket();
        void    listen_socket();
        void    creat_socket();
        SOCKET  get_socket() const;
        ~Socket();
};

#endif