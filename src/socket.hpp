#ifndef SOCKET_HPP
#define SOCKET_HPP


#include <string>

#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

using namespace std;


class Socket {

protected:
    string name;
    int fd;
    struct sockaddr_un addr;

public:
    Socket(string name);
    ~Socket();

    int get_fd() const;
    string get_name() const;
    struct sockaddr_un get_addr() const;

};


#endif
