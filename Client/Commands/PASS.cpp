#include "Server.hpp"

void Server::cmdPass(int fd, const std::vector<std::string> &params) {
    Client &client = _clients[fd];
    
    if (client.isRegistered()) {
        sendError(fd, ERR_ALREADYREGISTRED, ":You may not reregister");
        return;
    }
    
    if (params.empty()) {
        sendError(fd, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
        return;
    }
    
    if (params[0] == _password) {
        client.setPassOk(true);
        checkRegistration(fd);
    } else {
        sendError(fd, ERR_PASSWDMISMATCH, ":Password incorrect");
    }
}
