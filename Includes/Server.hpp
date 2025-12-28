#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <stdexcept>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>

class Server
{
private:
    int listener;
    std::vector<pollfd> pfds;

    int  get_listening_socket(const char *port);
    void add_fd(int fd);
    void make_non_blocking(int fd);
    void handle_new_connection();
    void handle_client(size_t &i);

    Server();

public:
    static Server &get_server()
    {
        static Server instance;
        return instance;
    }

    void server_run(const char *port);
    ~Server();
};
