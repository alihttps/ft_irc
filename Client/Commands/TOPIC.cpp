#include "Server.hpp"

void Server::cmdTopic(int fd, const std::vector<std::string> &params) {
    if (params.empty()) {
        sendError(fd, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
        return;
    }
    
    Client &client = _clients[fd];
    std::string chanName = params[0];
    
    if (_channels.find(chanName) == _channels.end()) {
        sendError(fd, ERR_NOSUCHCHANNEL, chanName + " :No such channel");
        return;
    }
    
    Channel &channel = _channels[chanName];
    
    if (!channel.isMember(fd)) {
        sendError(fd, ERR_NOTONCHANNEL, chanName + " :You're not on that channel");
        return;
    }
    
    if (params.size() == 1) {
        if (channel.getTopic().empty())
            sendReply(fd, RPL_NOTOPIC, chanName + " :No topic is set");
        else
            sendReply(fd, RPL_TOPIC, chanName + " :" + channel.getTopic());
        return;
    }
    
    if (channel.isTopicRestricted() && !channel.isOperator(fd)) {
        sendError(fd, ERR_CHANOPRIVSNEEDED, chanName + " :You're not channel operator");
        return;
    }
    
    channel.setTopic(params[1]);
    
    std::string topicMsg = ":" + client.getPrefix() + " TOPIC " + chanName + " :" + params[1];
    channel.broadcast(topicMsg);
}