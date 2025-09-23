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
        Server();
    public : 
        static void GetArgsToParse(const char ** const argv);
        ~Server();
};