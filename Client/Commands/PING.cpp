#include "Server.hpp"

void Server::cmdPing(int fd, const std::vector<std::string> &params) {
    std::string response = ":" + std::string(SERVER_NAME) + " PONG " + std::string(SERVER_NAME);
    if (!params.empty())
        response += " :" + params[0];
    _clients[fd].sendMessage(response);
}
