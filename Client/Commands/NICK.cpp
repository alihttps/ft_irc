#include "Server.hpp"

void Server::cmdNick(int fd, const std::vector<std::string> &params) {
    Client &client = _clients[fd];
    
    if (!client.isPassOk()) {
        sendError(fd, ERR_PASSWDMISMATCH, ":Password required");
        return;
    }
    
    if (params.empty()) {
        sendError(fd, ERR_NONICKNAMEGIVEN, ":No nickname given");
        return;
    }
    
    std::string newNick = params[0];
    
    if (!Client::isValidNickname(newNick)) {
        sendError(fd, ERR_ERRONEUSNICKNAME, newNick + " :Erroneous nickname");
        return;
    }
    
    Client *existing = getClientByNick(newNick);
    if (existing && existing->getFd() != fd) {
        sendError(fd, ERR_NICKNAMEINUSE, newNick + " :Nickname is already in use");
        return;
    }
    
    std::string oldNick = client.getNickname();
    client.setNickname(newNick);
    client.setNickSet(true);
    
    if (client.isRegistered()) {
        std::string msg = ":" + oldNick + "!" + client.getUsername() + "@" + client.getHostname() + 
                         " NICK :" + newNick;
        client.sendMessage(msg);
        
        std::set<std::string> channels = client.getChannels();
        for (std::set<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
            if (_channels.find(*it) != _channels.end())
                _channels[*it].broadcast(msg, fd);
        }
    }
    
    checkRegistration(fd);
}