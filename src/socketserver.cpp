#include "socketserver.hpp"


SocketServer::SocketServer(string name) : Socket(name) {
    // Remove old socket if it wasn't cleaned up
    unlink(name.c_str());

    int s_flags = fcntl(this->fd, F_GETFL);
    if (fcntl(this->fd, F_SETFL, s_flags | O_NONBLOCK) < 0)
        throw mucs_exception("Error making socket non-blocking");
}


SocketServer::~SocketServer() {
    unlink(name.c_str());
}


void SocketServer::bind() const {
    int ret = ::bind(
        this->fd,
        (const struct sockaddr*)&this->addr,
        sizeof(this->addr)
    );
    if (ret < 0)
        throw mucs_exception("Error binding to socket");
}


void SocketServer::listen(int backlog) const {
    if (::listen(this->fd, backlog) < 0)
        throw mucs_exception("Error listening");
}


int SocketServer::accept(struct sockaddr *addr, socklen_t *addrlen) const {
    int ret = ::accept(this->fd, addr, addrlen);
    if (ret < 0) {
        if (errno != EAGAIN || errno != EWOULDBLOCK)
            throw mucs_exception("Error accepting connections");
    }
    return ret;
}
