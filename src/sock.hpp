#ifndef SOCK_HPP
#define SOCK_HPP


#include <iostream>
#include <string>

#include <unistd.h>

#include <sys/time.h>

#include "socketclient.hpp"
#include "socketserver.hpp"
#include "util.hpp"

#define  SOCKET_NAME     "/tmp/mucs.socket"
#define  BUFFER_SIZE     128
#define  LISTEN_BACKLOG  128

using namespace std;


void host_registration();

void send_username(string user);


#endif
