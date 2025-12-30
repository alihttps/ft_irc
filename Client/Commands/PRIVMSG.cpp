#include "Server.hpp"

void Server::cmdPrivmsg(int fd, const std::vector<std::string> &params) {
    if (params.empty()) {
        sendError(fd, ERR_NORECIPIENT, ":No recipient given (PRIVMSG)");
        return;
    }
    
    if (params.size() < 2) {
        sendError(fd, ERR_NOTEXTTOSEND, ":No text to send");
        return;
    }
    
    Client &client = _clients[fd];
    std::string target = params[0];
    std::string message = params[1];
    std::string msg = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + message;
    
    if (target[0] == '#' || target[0] == '&') {
        if (_channels.find(target) == _channels.end()) {
            sendError(fd, ERR_NOSUCHCHANNEL, target + " :No such channel");
            return;
        }
        
        Channel &channel = _channels[target];
        
        if (!channel.isMember(fd)) {
            sendError(fd, ERR_CANNOTSENDTOCHAN, target + " :Cannot send to channel");
            return;
        }
        
        channel.broadcast(msg, fd);
    } else {
        int targetFd = getClientFdByNick(target);
        if (targetFd == -1) {
            sendError(fd, ERR_NOSUCHNICK, target + " :No such nick/channel");
            return;
        }
        
        _clients[targetFd].sendMessage(msg);
    }
}
