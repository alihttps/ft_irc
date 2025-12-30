#include "Server.hpp"

void Server::cmdPart(int fd, const std::vector<std::string> &params) {
    if (params.empty()) {
        sendError(fd, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
        return;
    }
    
    Client &client = _clients[fd];
    std::vector<std::string> channelNames = Client::split(params[0], ',');
    std::string reason = (params.size() > 1) ? params[1] : "";
    
    for (size_t i = 0; i < channelNames.size(); i++) {
        std::string chanName = channelNames[i];
        
        if (_channels.find(chanName) == _channels.end()) {
            sendError(fd, ERR_NOSUCHCHANNEL, chanName + " :No such channel");
            continue;
        }
        
        Channel &channel = _channels[chanName];
        
        if (!channel.isMember(fd)) {
            sendError(fd, ERR_NOTONCHANNEL, chanName + " :You're not on that channel");
            continue;
        }
        
        std::string partMsg = ":" + client.getPrefix() + " PART " + chanName;
        if (!reason.empty())
            partMsg += " :" + reason;
        
        channel.broadcast(partMsg);
        channel.removeMember(fd);
        client.leaveChannel(chanName);
        
        if (channel.isEmpty())
            _channels.erase(chanName);
    }
}
