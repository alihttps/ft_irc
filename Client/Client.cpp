#include "Client.hpp"
#include "Server.hpp"
#include <algorithm>

#define MSG_MAX 512

std::vector<std::string> splitset(std::string buff)
{
    std::vector<std::string> data; 
    int hold = 0;
    int i;
    while(buff[hold] == ' ')
        hold++;
    i = hold;
    for ( ; buff[i]; i++)
    {
        if (buff[i] == ' ')
        {
            data.push_back(buff.substr(hold, i - hold));
            while(buff[i] == ' ')
                i++;
            hold = i;
        }
    }
    if(!buff[i])
    {
        while(buff[i] != ' ')
            i--;
        data.push_back(buff.substr(i + 1 , buff.size()));
    }
    data.erase(std::find(data.begin(), data.end(), ""));  
    return (data);
}
Client::Client(int fd, char *password) : _fd(fd), _password(password)
{
}
std::string Client::ClientInvoke()
{
    // char buff[513];
    // buff[MSG_MAX + 1] = '\0';
    // recv(_fd, buff, MSG_MAX, 0);
    // std::string result(buff);
    std::vector<std::string> newer_model;
    std::string  nuff = "            hello world and   asdasd: : 123123  asdasdasdasd everyone      ";
    newer_model = splitset(nuff);
    for (std::vector<std::string>::iterator it = newer_model.begin(); it != newer_model.end(); it++)
        std::cout << *it << std::endl;
    // registration(result);
    // return result;
    return "";
}

Client::~Client()
{
}