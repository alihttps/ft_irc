#include "Server.hpp"

void Server::cmdQuit(int fd, const std::vector<std::string> &params) {
    std::string reason = (params.empty()) ? "Leaving" : params[0];
    
    Client &client = _clients[fd];
    std::string quitMsg = ":" + client.getPrefix() + " QUIT :" + reason;
    
    std::set<std::string> channels = client.getChannels();
    for (std::set<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (_channels.find(*it) != _channels.end())
            _channels[*it].broadcast(quitMsg, fd);
    }
    
    removeClient(fd);
}
