#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cstring>
#include <stdexcept>
#include <cstdlib>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>

#include "Client.hpp"
#include "Channel.hpp"

#define RPL_WELCOME         "001"
#define RPL_YOURHOST        "002"
#define RPL_CREATED         "003"
#define RPL_MYINFO          "004"
#define RPL_CHANNELMODEIS   "324"
#define RPL_NOTOPIC         "331"
#define RPL_TOPIC           "332"
#define RPL_INVITING        "341"
#define RPL_NAMREPLY        "353"
#define RPL_ENDOFNAMES      "366"

#define ERR_NOSUCHNICK      "401"
#define ERR_NOSUCHCHANNEL   "403"
#define ERR_CANNOTSENDTOCHAN "404"
#define ERR_TOOMANYCHANNELS "405"
#define ERR_NORECIPIENT     "411"
#define ERR_NOTEXTTOSEND    "412"
#define ERR_UNKNOWNCOMMAND  "421"
#define ERR_NONICKNAMEGIVEN "431"
#define ERR_ERRONEUSNICKNAME "432"
#define ERR_NICKNAMEINUSE   "433"
#define ERR_USERNOTINCHANNEL "441"
#define ERR_NOTONCHANNEL    "442"
#define ERR_USERONCHANNEL   "443"
#define ERR_NOTREGISTERED   "451"
#define ERR_NEEDMOREPARAMS  "461"
#define ERR_ALREADYREGISTRED "462"
#define ERR_PASSWDMISMATCH  "464"
#define ERR_CHANNELISFULL   "471"
#define ERR_INVITEONLYCHAN  "473"
#define ERR_BADCHANNELKEY   "475"
#define ERR_CHANOPRIVSNEEDED "482"

#define SERVER_NAME "ircserv"

class Server
{
private:
    int                         _listener;
    std::string                 _password;
    std::string                 _port;
    std::vector<pollfd>         _pfds;
    std::map<int, Client>       _clients;
    std::map<std::string, Channel> _channels;

    int     getListeningSocket(const std::string &port);
    void    addFd(int fd);
    void    removeFd(int fd);
    void    makeNonBlocking(int fd);
    void    handleNewConnection();
    void    handleClientData(int fd);
    void    removeClient(int fd);
    void    routeCommand(int fd, const std::string &line);
    void    checkRegistration(int fd);
    void    sendWelcome(int fd);

    void    cmdPass(int fd, const std::vector<std::string> &params);
    void    cmdNick(int fd, const std::vector<std::string> &params);
    void    cmdUser(int fd, const std::vector<std::string> &params);
    void    cmdJoin(int fd, const std::vector<std::string> &params);
    void    cmdPart(int fd, const std::vector<std::string> &params);
    void    cmdPrivmsg(int fd, const std::vector<std::string> &params);
    void    cmdKick(int fd, const std::vector<std::string> &params);
    void    cmdInvite(int fd, const std::vector<std::string> &params);
    void    cmdTopic(int fd, const std::vector<std::string> &params);
    void    cmdMode(int fd, const std::vector<std::string> &params);
    void    cmdQuit(int fd, const std::vector<std::string> &params);
    void    cmdPing(int fd, const std::vector<std::string> &params);

    void    sendReply(int fd, const std::string &code, const std::string &message);
    void    sendError(int fd, const std::string &code, const std::string &message);
    Client* getClientByNick(const std::string &nickname);
    int     getClientFdByNick(const std::string &nickname);

    Server();
    Server(const Server &other);
    Server &operator=(const Server &other);

public:
    static Server &getInstance();
    void    run(const std::string &port, const std::string &password);
    void    stop();
    ~Server();

    Client* getClient(int fd);
};

