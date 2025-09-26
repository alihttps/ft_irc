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
        std::vector<struct pollfd> pfds;
        int fd_count;
        int listener;
        void add_to_pfds(const int &newfd);
        void del_from_pfds(const int &i);
        void handle_new_connection();
        void handle_client(const int &i);
        int  set_listening_socket();
        void process_connections();
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

