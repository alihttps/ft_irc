#include "Server.hpp"

void Server::cmdMode(int fd, const std::vector<std::string> &params) {
    if (params.empty()) {
        sendError(fd, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
        return;
    }
    
    std::string target = params[0];
    
    if (target[0] != '#' && target[0] != '&')
        return;
    
    std::string chanName = target;
    
    if (_channels.find(chanName) == _channels.end()) {
        sendError(fd, ERR_NOSUCHCHANNEL, chanName + " :No such channel");
        return;
    }
    
    Channel &channel = _channels[chanName];
    Client &client = _clients[fd];
    
    if (params.size() == 1) {
        std::string modes = channel.getModeString();
        sendReply(fd, RPL_CHANNELMODEIS, chanName + " " + modes);
        return;
    }
    
    if (!channel.isOperator(fd)) {
        sendError(fd, ERR_CHANOPRIVSNEEDED, chanName + " :You're not channel operator");
        return;
    }
    
    std::string modeStr = params[1];
    bool adding = true;
    size_t paramIdx = 2;
    std::string appliedModes = "";
    std::string appliedParams = "";
    
    for (size_t i = 0; i < modeStr.length(); i++) {
        char c = modeStr[i];
        
        if (c == '+') {
            adding = true;
            continue;
        }
        if (c == '-') {
            adding = false;
            continue;
        }
        
        switch (c) {
            case 'i':
                channel.setInviteOnly(adding);
                appliedModes += (adding ? "+i" : "-i");
                break;
            
            case 't':
                channel.setTopicRestricted(adding);
                appliedModes += (adding ? "+t" : "-t");
                break;
            
            case 'k':
                if (adding) {
                    if (paramIdx < params.size()) {
                        channel.setKey(params[paramIdx]);
                        appliedModes += "+k";
                        appliedParams += " " + params[paramIdx];
                        paramIdx++;
                    }
                } else {
                    channel.setKey("");
                    appliedModes += "-k";
                }
                break;
            
            case 'o':
                if (paramIdx < params.size()) {
                    int targetFd = getClientFdByNick(params[paramIdx]);
                    if (targetFd != -1 && channel.isMember(targetFd)) {
                        if (adding)
                            channel.addOperator(targetFd);
                        else
                            channel.removeOperator(targetFd);
                        appliedModes += (adding ? "+o" : "-o");
                        appliedParams += " " + params[paramIdx];
                    }
                    paramIdx++;
                }
                break;
            
            case 'l':
                if (adding) {
                    if (paramIdx < params.size()) {
                        int limit = std::atoi(params[paramIdx].c_str());
                        if (limit > 0) {
                            channel.setUserLimit(limit);
                            appliedModes += "+l";
                            appliedParams += " " + params[paramIdx];
                        }
                        paramIdx++;
                    }
                } else {
                    channel.setUserLimit(0);
                    appliedModes += "-l";
                }
                break;
        }
    }
    
    if (!appliedModes.empty()) {
        std::string modeMsg = ":" + client.getPrefix() + " MODE " + chanName + " " + appliedModes + appliedParams;
        channel.broadcast(modeMsg);
    }
}
