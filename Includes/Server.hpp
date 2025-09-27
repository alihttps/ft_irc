#pragma once
#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>
#include <list>
#include <deque>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <cstring>
#include <arpa/inet.h>
#include <poll.h>
#include <sstream>
#include <signal.h>
#include <exception>

class Server
{
    private :
        struct addrinfo hints, *ai, *p;
        struct sockaddr_storage remoteaddr;
        std::vector<struct pollfd> pfds;
        int fd_size;
        int fd_count;
        int listener;
        void add_to_pfds(int *newfd, int *fd_count, int *fd_size);
        void del_from_pfds(int i ,int *fd_count);
        void handle_new_connection (int listener, int *fd_count, int *fd_size);
        void handle_client(int listener, int *fd_count, int *i);
        void process_connections(int listener, int *fd_count, int *fd_size);
        static void GetArgsToParse(const char ** const argv);
        Server();
        
    public :
        static Server& get_server()
        {
            static Server server;
            return server;
        }
        void server_init();
        ~Server();
};

