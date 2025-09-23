#include "Server.hpp"

Server::Server()
{
}
bool check_portvalidity(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if(!isdigit(str[i]) || i > 5)
            return (false);
    }
    return (true);
}
bool check_passwordvalidity(const char *str)
{
    for(int i = 0; str[i];  i++)
    {
        if(!isprint(str[i]) || i > 500)
            return (false);
    }
    return (true);
}
void Server::GetArgsToParse(const char ** const argv)
{
    if(!check_portvalidity(argv[1]))
        throw std::runtime_error("incompatible port type of characters or out of range");
    if(!check_passwordvalidity(argv[2]))
        throw std::runtime_error("invalid password charcter or characters are to long");
}

// void server_init()
// {
//     int listener;

//     int fd_size = 5;
//     int fd_count = 0;
    
//     struct pollfd *pfds = (struct pollfd*) malloc(sizeof(struct pollfd) * fd_size);



// }