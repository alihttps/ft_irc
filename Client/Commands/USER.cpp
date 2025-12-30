#include "Server.hpp"

void Server::cmdUser(int fd, const std::vector<std::string> &params) {
    Client &client = _clients[fd];
    
    if (client.isRegistered()) {
        sendError(fd, ERR_ALREADYREGISTRED, ":You may not reregister");
        return;
    }
    
    if (!client.isPassOk()) {
        sendError(fd, ERR_PASSWDMISMATCH, ":Password required");
        return;
    }
    
    if (params.size() < 4) {
        sendError(fd, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
        return;
    }
    
    client.setUsername(params[0]);
    client.setRealname(params[3]);
    client.setUserSet(true);
    
    checkRegistration(fd);
}
