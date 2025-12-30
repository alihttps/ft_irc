#include "Server.hpp"

Server::Server() : _listener(-1) {}

Server::Server(const Server &other) { (void)other; }

Server &Server::operator=(const Server &other) { (void)other; return *this; }

Server::~Server() {
    stop();
}

Server &Server::getInstance() {
    static Server instance;
    return instance;
}

void Server::makeNonBlocking(int fd) {
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "fcntl failed on fd " << fd << std::endl;
    }
}

int Server::getListeningSocket(const std::string &port) {
    int listener;
    int yes = 1;
    struct addrinfo hints, *ai, *p;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    int rv = getaddrinfo(NULL, port.c_str(), &hints, &ai);
    if (rv != 0)
        throw std::runtime_error(gai_strerror(rv));
        
    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
            continue;
        makeNonBlocking(listener);
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        
        if (bind(listener, p->ai_addr, p->ai_addrlen) == 0)
            break;
        
        close(listener);
    }
        
    if (!p) {
        freeaddrinfo(ai);
        throw std::runtime_error("failed to bind socket");
    }
    
    freeaddrinfo(ai);
        
    if (listen(listener, SOMAXCONN) < 0)
        throw std::runtime_error("listen failed");
    
    return listener;
}

void Server::addFd(int fd) {
    pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pfds.push_back(pfd);
}

void Server::removeFd(int fd) {
    for (std::vector<pollfd>::iterator it = _pfds.begin(); it != _pfds.end(); ++it) {
        if (it->fd == fd) {
            _pfds.erase(it);
            break;
        }
    }
}

void Server::handleNewConnection() {
    sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);

    int newfd = accept(_listener, (sockaddr *)&remoteaddr, &addrlen);
    if (newfd < 0)
        return;

    makeNonBlocking(newfd);
    addFd(newfd);
    
    Client client(newfd);
    char ip[INET_ADDRSTRLEN];
    sockaddr_in *s = (sockaddr_in *)&remoteaddr;
    inet_ntop(AF_INET, &s->sin_addr, ip, sizeof(ip));
    client.setHostname(ip);
    
    _clients[newfd] = client;

    std::cout << "New connection from " << ip << " on socket " << newfd << std::endl;
}

void Server::handleClientData(int fd) {
    char buffer[4096];
    int nbytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (nbytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        nbytes = 0;
    }

    if (nbytes == 0) {
        std::cout << "Socket " << fd << " disconnected" << std::endl;
        removeClient(fd);
        return;
    }

    buffer[nbytes] = '\0';
    
    if (_clients.find(fd) == _clients.end())
        return;
    
    _clients[fd].appendToBuffer(buffer);
    
    std::string line;
    while (_clients[fd].extractLine(line)) {
        if (!line.empty()) {
            std::cout << "Received from " << fd << ": " << line << std::endl;
            routeCommand(fd, line);
        }
    }
}

void Server::removeClient(int fd) {
    if (_clients.find(fd) == _clients.end())
        return;
    
    std::set<std::string> channels = _clients[fd].getChannels();
    for (std::set<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (_channels.find(*it) != _channels.end()) {
            std::string quitMsg = ":" + _clients[fd].getPrefix() + " QUIT :Client disconnected";
            _channels[*it].broadcast(quitMsg, fd);
            _channels[*it].removeMember(fd);
            if (_channels[*it].isEmpty())
                _channels.erase(*it);
        }
    }
    
    _clients.erase(fd);
    removeFd(fd);
    close(fd);
}

void Server::routeCommand(int fd, const std::string &line) {
    if (line.empty())
        return;
    
    std::string command;
    std::vector<std::string> params;
    Client::parseIRCLine(line, command, params);
    
    if (command.empty())
        return;
    
    CommandType cmdType = Client::getCommandType(command);
    
    if (!_clients[fd].isRegistered() && Client::requiresRegistration(cmdType)) {
        sendError(fd, ERR_NOTREGISTERED, ":You have not registered");
        return;
    }
    
    switch (cmdType) {
        case CMD_PASS:    cmdPass(fd, params);    break;
        case CMD_NICK:    cmdNick(fd, params);    break;
        case CMD_USER:    cmdUser(fd, params);    break;
        case CMD_JOIN:    cmdJoin(fd, params);    break;
        case CMD_PART:    cmdPart(fd, params);    break;
        case CMD_PRIVMSG: cmdPrivmsg(fd, params); break;
        case CMD_KICK:    cmdKick(fd, params);    break;
        case CMD_INVITE:  cmdInvite(fd, params);  break;
        case CMD_TOPIC:   cmdTopic(fd, params);   break;
        case CMD_MODE:    cmdMode(fd, params);    break;
        case CMD_QUIT:    cmdQuit(fd, params);    break;
        case CMD_PING:    cmdPing(fd, params);    break;
        case CMD_CAP:     break;
        default:
            if (_clients[fd].isRegistered())
                sendError(fd, ERR_UNKNOWNCOMMAND, command + " :Unknown command");
            break;
    }
}

void Server::sendReply(int fd, const std::string &code, const std::string &message) {
    _clients[fd].sendReply(SERVER_NAME, code, message);
}

void Server::sendError(int fd, const std::string &code, const std::string &message) {
    sendReply(fd, code, message);
}

Client* Server::getClientByNick(const std::string &nickname) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickname() == nickname)
            return &it->second;
    }
    return NULL;
}

int Server::getClientFdByNick(const std::string &nickname) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickname() == nickname)
            return it->first;
    }
    return -1;
}

Client* Server::getClient(int fd) {
    if (_clients.find(fd) != _clients.end())
        return &_clients[fd];
    return NULL;
}

void Server::checkRegistration(int fd) {
    Client &client = _clients[fd];
    
    if (client.tryRegister())
        sendWelcome(fd);
}

void Server::sendWelcome(int fd) {
    Client &client = _clients[fd];
    
    sendReply(fd, RPL_WELCOME, ":Welcome to the IRC Network " + client.getPrefix());
    sendReply(fd, RPL_YOURHOST, ":Your host is " + std::string(SERVER_NAME) + ", running version 1.0");
    sendReply(fd, RPL_CREATED, ":This server was created today");
    sendReply(fd, RPL_MYINFO, std::string(SERVER_NAME) + " 1.0 o itkol");
}

void Server::run(const std::string &port, const std::string &password) {
    _port = port;
    _password = password;
    _listener = getListeningSocket(port);
    addFd(_listener);

    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
        if (poll(_pfds.data(), _pfds.size(), -1) < 0) {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("poll failed");
        }

        for (size_t i = 0; i < _pfds.size(); i++) {
            if (_pfds[i].revents & POLLIN) {
                if (_pfds[i].fd == _listener) {
                    handleNewConnection();
                } else {
                    handleClientData(_pfds[i].fd);
                }
            }
        }
    }
}

void Server::stop() {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        close(it->first);
    }
    _clients.clear();
    _channels.clear();
    
    if (_listener >= 0) {
        close(_listener);
        _listener = -1;
    }
    _pfds.clear();
}
