#include "Server.hpp"

Server::Server() : listener(-1) {}

<<<<<<< HEAD:irc/Server/server.cpp
Server::Server() : fd_count(0), listener(-1)
{
    
}


bool check_portvalidity(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if(!isdigit(str[i]) || i > 5)
            return (false);
    } 
    return (atoi(str) > 65535);
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
    _port = argv[1];
}


int Server::set_listening_socket()
=======
void Server::make_non_blocking(int fd) {
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "fcntl failed on fd " << fd << std::endl;
    }
}

int Server::get_listening_socket(const char *port)
>>>>>>> robo:Server/server.cpp
{
    int yes = 1;
<<<<<<< HEAD:irc/Server/server.cpp
    int rv;
    char ipstr[INET_ADDRSTRLEN];
=======
    struct addrinfo hints, *ai, *p;
>>>>>>> robo:Server/server.cpp

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

<<<<<<< HEAD:irc/Server/server.cpp
    if (rv = getaddrinfo(NULL, PORT, &hints, &ai) != 0)
=======
    int rv = getaddrinfo(NULL, port, &hints, &ai);
    if (rv != 0)
>>>>>>> robo:Server/server.cpp
        throw std::runtime_error(gai_strerror(rv));

    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
            continue;
        make_non_blocking(listener);
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
<<<<<<< HEAD:irc/Server/server.cpp
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener);
            continue;
        }
        struct sockaddr_in *addr_in = (struct sockaddr_in *)p->ai_addr;
        inet_ntop(AF_INET, &(addr_in->sin_addr), ipstr, sizeof(ipstr));
        std::cout << "Listening on " << ipstr << ":" << PORT << std::endl;

        break;
=======

        if (bind(listener, p->ai_addr, p->ai_addrlen) == 0)
            break;

        close(listener);
>>>>>>> robo:Server/server.cpp
    }

    if (!p)
        throw std::runtime_error("failed to bind socket");

    freeaddrinfo(ai);

    if (listen(listener, SOMAXCONN) < 0)
        throw std::runtime_error("listen failed");

    return listener;
}

<<<<<<< HEAD:irc/Server/server.cpp
void Server::add_to_pfds(const int &newfd)
{
    struct pollfd pfd;
    pfd.fd = newfd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    pfds.push_back(pfd);
    fd_count++;

}

=======

void Server::add_fd(int fd)
{
    pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    pfds.push_back(pfd);
}


>>>>>>> robo:Server/server.cpp
void Server::handle_new_connection()
{
    sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);

    int newfd = accept(listener, (sockaddr *)&remoteaddr, &addrlen);
    if (newfd < 0)
<<<<<<< HEAD:irc/Server/server.cpp
        throw std::runtime_error("accept error");
    std::cout << "new connection from " << inet_ntop(remoteaddr.ss_family, &(s->sin_addr), remoteIP, sizeof(remoteIP)) << " on socket " << newfd << std::endl;
    
    add_to_pfds(newfd);
}

void Server::del_from_pfds(const int &i)
=======
        return;

    make_non_blocking(newfd);
    add_fd(newfd);
    char ip[INET_ADDRSTRLEN];
    sockaddr_in *s = (sockaddr_in *)&remoteaddr;
    inet_ntop(AF_INET, &s->sin_addr, ip, sizeof(ip));

    std::cout << "New connection from " << ip
              << " on socket " << newfd << std::endl;
}


void Server::handle_client(size_t &i)
>>>>>>> robo:Server/server.cpp
{
char buffer[4096];
    int fd = pfds[i].fd;

<<<<<<< HEAD:irc/Server/server.cpp
    fd_count--;
}

void Server::handle_client(const int &i)
{
    char buffer[4096];
    
    int nbytes = recv(pfds[i].fd, buffer, sizeof(buffer), 0);

    const int sender_fd = pfds[i].fd;
=======
    int nbytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (nbytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) 
        {
            i++;
            return;
        }
        nbytes = 0; 
    }

    if (nbytes == 0) {
        std::cout << "Socket " << fd << " disconnected\n";
        close(fd);
        pfds.erase(pfds.begin() + i);
        return; 
    }
>>>>>>> robo:Server/server.cpp

    buffer[nbytes] = '\0';
    std::cout << "Received from " << fd << ": " << buffer;

    for (size_t j = 0; j < pfds.size(); j++)
    {
<<<<<<< HEAD:irc/Server/server.cpp
        if (nbytes == 0) //closed connection
        {
            std::cout << "socket " << pfds[i].fd << " hung up\n";
        }
        else
            perror("recv");
        
        close(pfds[i].fd);
        del_from_pfds(i);
    }
    else
    {
        std::cout << "server : received from " << pfds[i].fd << " :" << buffer;

        for (int j = 0; j < fd_count; j++)
        {
            int dest_fd = pfds[j].fd;

            if (pfds[j].fd != listener && pfds[j].fd != sender_fd)
            {
                int s_bytes = send(dest_fd, buffer, nbytes, 0);
                if (s_bytes < 0)
                    throw std::runtime_error("send fail");
            }

        }
        
    }
        
}

void Server::process_connections()
{
    for (int i = 0; i < fd_count; i++)
    { 
        if (pfds[i].revents & POLLIN)
        {
            if (pfds[i].fd == listener)
            {
                handle_new_connection(); //if we r the listener its a new connection
            }
            else
                handle_client(i); // regular client
        }
    }
    
}

void Server::server_init() 
{
    listener = set_listening_socket();
    if (listener == -1)
        throw std::runtime_error("error in listening socket\n");
    fd_count = 1;

    struct pollfd pfd;
    pfd.fd = listener;
    pfd.events = POLLIN;
    pfd.revents = 0;

    pfds.push_back(pfd);

    std::cout << "server waiting for connection ..." << std::endl;

    while (true)
    {
        int pollcount = poll(&pfds[0], fd_count, -1);
        if (pollcount == -1)
            throw std::runtime_error("poll\n");
        
        process_connections();
=======
        if (pfds[j].fd != listener && pfds[j].fd != fd)
            send(pfds[j].fd, buffer, nbytes, 0);
    }

    i++;
}


void Server::server_run(const char *port)
{
    listener = get_listening_socket(port);
    add_fd(listener);

    std::cout << "Server listening on port " << port << std::endl;

    while (true)
    {
        if (poll(pfds.data(), pfds.size(), -1) < 0)
            throw std::runtime_error("poll failed");

        for (size_t i = 0; i < pfds.size(); )
        {
            if (pfds[i].revents & POLLIN)
            {
                if (pfds[i].fd == listener)
                {
                    handle_new_connection();
                    i++;
                }
                else
                    handle_client(i);
            }
            else
                i++;
        }
>>>>>>> robo:Server/server.cpp
    }
}


Server::~Server()
{
    for (size_t i = 0; i < pfds.size(); i++)
        close(pfds[i].fd);
}
