#include "Server.hpp"

#define PORT "8080"

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


int get_listening_socket()
{
    int listener;
    int yes = 1;
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (rv = getaddrinfo(NULL, PORT, &hints, &ai) != 0)
        throw std::runtime_error(gai_strerror(rv));
    
    for (p = ai; p != NULL ; p = ai->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
            continue;
        
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener);
            continue;
        }
        break;
    }

    if (!p)
        throw std::runtime_error("couldnt get bound\n");

    if (listen(listener, SOMAXCONN) < 0)
        throw std::runtime_error("listening error\n");
    
    return listener;
}

void server_init()
{
    int listener;

    int fd_size = 5;
    int fd_count = 0;
    
    struct pollfd *pfds = (struct pollfd*) malloc(sizeof(struct pollfd) * fd_size);

    listener = get_listening_socket();
    if (listener == -1)
        throw std::runtime_error("error in listening socket\n");
    fd_count = 1;

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    std::cout << "server waiting for connection ..." << std::endl;

    while (true)
    {
        int pollcount = poll(pfds, fd_count, -1);
        if (pollcount == -1)
            throw std::runtime_error("poll\n");
        
        process_connections(listener, &fd_count, &fd_size, &pfds);
    }
}