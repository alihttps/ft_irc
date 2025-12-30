#include "Server.hpp"

void Server::cmdJoin(int fd, const std::vector<std::string> &params) {
    if (params.empty()) {
        sendError(fd, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
        return;
    }
    
    Client &client = _clients[fd];
    std::vector<std::string> channelNames = Client::split(params[0], ',');
    std::vector<std::string> keys;
    if (params.size() > 1)
        keys = Client::split(params[1], ',');
    
    for (size_t i = 0; i < channelNames.size(); i++) {
        std::string chanName = channelNames[i];
        std::string key = (i < keys.size()) ? keys[i] : "";
        
        if (chanName.empty() || (chanName[0] != '#' && chanName[0] != '&')) {
            sendError(fd, ERR_NOSUCHCHANNEL, chanName + " :No such channel");
            continue;
        }
        
        bool newChannel = (_channels.find(chanName) == _channels.end());
        if (newChannel)
            _channels[chanName] = Channel(chanName);
        
        Channel &channel = _channels[chanName];
        
        if (channel.isMember(fd))
            continue;
        
        if (channel.isInviteOnly() && !channel.isInvited(client.getNickname())) {
            sendError(fd, ERR_INVITEONLYCHAN, chanName + " :Cannot join channel (+i)");
            continue;
        }
        
        if (!channel.getKey().empty() && channel.getKey() != key) {
            sendError(fd, ERR_BADCHANNELKEY, chanName + " :Cannot join channel (+k)");
            continue;
        }
        
        if (channel.getUserLimit() > 0 && channel.getMemberCount() >= channel.getUserLimit()) {
            sendError(fd, ERR_CHANNELISFULL, chanName + " :Cannot join channel (+l)");
            continue;
        }
        
        channel.addMember(fd);
        client.joinChannel(chanName);
        channel.removeInvite(client.getNickname());
        
        if (newChannel)
            channel.addOperator(fd);
        
        std::string joinMsg = ":" + client.getPrefix() + " JOIN " + chanName;
        channel.broadcast(joinMsg);
        
        if (!channel.getTopic().empty())
            sendReply(fd, RPL_TOPIC, chanName + " :" + channel.getTopic());
        else
            sendReply(fd, RPL_NOTOPIC, chanName + " :No topic is set");
        
        std::string names = "";
        const std::set<int> &members = channel.getMembers();
        for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it) {
            if (!names.empty())
                names += " ";
            if (channel.isOperator(*it))
                names += "@";
            names += _clients[*it].getNickname();
        }
        sendReply(fd, RPL_NAMREPLY, "= " + chanName + " :" + names);
        sendReply(fd, RPL_ENDOFNAMES, chanName + " :End of /NAMES list");
    }
}