#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP


#include <string>

#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>

#include "exc.hpp"
#include "socket.hpp"

using namespace std;


class SocketServer : public Socket {

public:
    SocketServer(string name);
    ~SocketServer();

    void bind() const;
    void listen(int backlog = LISTEN_BACKLOG) const;
    int accept(
        struct sockaddr *addr = nullptr,
        socklen_t *addrlen = nullptr
    ) const;

};


#endif
