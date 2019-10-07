#include "socketclient.hpp"


SocketClient::SocketClient(string name) : Socket(name) {
}


int SocketClient::connect() const {
    int ret = ::connect(
        this->fd,
        (const struct sockaddr*)&this->addr,
        sizeof(this->addr)
    );
    if (ret < 0)
        throw mucs_exception("No server listening");
    return ret;
}


int SocketClient::write(string data) const {
    int ret = ::write(this->fd, data.c_str(), data.size() + 1);
    if (ret < 0)
        throw mucs_exception("Error writing data");
    return ret;
}
