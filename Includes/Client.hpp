#pragma once
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <sys/types.h>

class Client
{
    private:
        int _fd;
        char *_password;
        std::vector<std::string> data;
    public:
        Client(int, char *);
        std::string ClientInvoke();
        size_t getContentLength(int fd);
        ~Client();
};