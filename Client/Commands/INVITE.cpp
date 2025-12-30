#include "Server.hpp"

void Server::cmdInvite(int fd, const std::vector<std::string> &params) {
    if (params.size() < 2) {
        sendError(fd, ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
        return;
    }
    
    Client &client = _clients[fd];
    std::string targetNick = params[0];
    std::string chanName = params[1];
    
    int targetFd = getClientFdByNick(targetNick);
    if (targetFd == -1) {
        sendError(fd, ERR_NOSUCHNICK, targetNick + " :No such nick/channel");
        return;
    }
    
    if (_channels.find(chanName) == _channels.end()) {
        sendError(fd, ERR_NOSUCHCHANNEL, chanName + " :No such channel");
        return;
    }
    
    Channel &channel = _channels[chanName];
    
    if (!channel.isMember(fd)) {
        sendError(fd, ERR_NOTONCHANNEL, chanName + " :You're not on that channel");
        return;
    }
    
    if (channel.isInviteOnly() && !channel.isOperator(fd)) {
        sendError(fd, ERR_CHANOPRIVSNEEDED, chanName + " :You're not channel operator");
        return;
    }
    
    if (channel.isMember(targetFd)) {
        sendError(fd, ERR_USERONCHANNEL, targetNick + " " + chanName + " :is already on channel");
        return;
    }
    
    channel.addInvite(targetNick);
    
    sendReply(fd, RPL_INVITING, targetNick + " " + chanName);
    
    std::string inviteMsg = ":" + client.getPrefix() + " INVITE " + targetNick + " :" + chanName;
    _clients[targetFd].sendMessage(inviteMsg);
}
