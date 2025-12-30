#include "Server.hpp"

void Server::cmdKick(int fd, const std::vector<std::string> &params) {
    if (params.size() < 2) {
        sendError(fd, ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
        return;
    }
    
    Client &client = _clients[fd];
    std::string chanName = params[0];
    std::string targetNick = params[1];
    std::string reason = (params.size() > 2) ? params[2] : client.getNickname();
    
    if (_channels.find(chanName) == _channels.end()) {
        sendError(fd, ERR_NOSUCHCHANNEL, chanName + " :No such channel");
        return;
    }
    
    Channel &channel = _channels[chanName];
    
    if (!channel.isMember(fd)) {
        sendError(fd, ERR_NOTONCHANNEL, chanName + " :You're not on that channel");
        return;
    }
    
    if (!channel.isOperator(fd)) {
        sendError(fd, ERR_CHANOPRIVSNEEDED, chanName + " :You're not channel operator");
        return;
    }
    
    int targetFd = getClientFdByNick(targetNick);
    if (targetFd == -1 || !channel.isMember(targetFd)) {
        sendError(fd, ERR_USERNOTINCHANNEL, targetNick + " " + chanName + " :They aren't on that channel");
        return;
    }
    
    std::string kickMsg = ":" + client.getPrefix() + " KICK " + chanName + " " + targetNick + " :" + reason;
    channel.broadcast(kickMsg);
    
    channel.removeMember(targetFd);
    _clients[targetFd].leaveChannel(chanName);
    
    if (channel.isEmpty())
        _channels.erase(chanName);
}