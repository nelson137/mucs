#include "socket.hpp"


Socket::Socket(string name) {
    this->name = name;

    this->fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (this->fd < 0)
        die("Error creating socket");

    int path_size = sizeof(this->addr.sun_path);
    int len = name.copy(this->addr.sun_path, path_size);
    this->addr.sun_path[len] = 0;
    this->addr.sun_family = AF_UNIX;
}


Socket::~Socket() {
    close(this->fd);
}


int Socket::get_fd() const {
    return this->fd;
}


string Socket::get_name() const {
    return this->name;
}


struct sockaddr_un Socket::get_addr() const {
    return this->addr;
}
