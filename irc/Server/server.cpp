#include "Server.hpp"

Server::Server() : listener(-1) {}

void Server::make_non_blocking(int fd) {
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "fcntl failed on fd " << fd << std::endl;
    }
}

int Server::get_listening_socket(const char *port)
{
    int yes = 1;
    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv = getaddrinfo(NULL, port, &hints, &ai);
    if (rv != 0)
        throw std::runtime_error(gai_strerror(rv));

    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
            continue;
        make_non_blocking(listener);
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) == 0)
            break;

        close(listener);
    }

    if (!p)
        throw std::runtime_error("failed to bind socket");

    freeaddrinfo(ai);

    if (listen(listener, SOMAXCONN) < 0)
        throw std::runtime_error("listen failed");

    return listener;
}


void Server::add_fd(int fd)
{
    pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    pfds.push_back(pfd);
}


void Server::handle_new_connection()
{
    sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);

    int newfd = accept(listener, (sockaddr *)&remoteaddr, &addrlen);
    if (newfd < 0)
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
{
char buffer[4096];
    int fd = pfds[i].fd;

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

    buffer[nbytes] = '\0';
    std::cout << "Received from " << fd << ": " << buffer;

    for (size_t j = 0; j < pfds.size(); j++)
    {
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
    }
}


Server::~Server()
{
    for (size_t i = 0; i < pfds.size(); i++)
        close(pfds[i].fd);
}
