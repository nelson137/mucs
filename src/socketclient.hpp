#ifndef SOCKET_CLIENT_HPP
#define SOCKET_CLIENT_HPP


#include <string>

#include <unistd.h>

#include <sys/socket.h>

#include "exc.hpp"
#include "socket.hpp"

using namespace std;


class SocketClient : public Socket {

public:
    SocketClient(string name);

    int connect() const;
    int write(string data) const;

};


#endif
