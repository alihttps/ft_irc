#pragma once

#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <sys/socket.h>

enum CommandType {
    CMD_UNKNOWN = 0,
    CMD_PASS,
    CMD_NICK,
    CMD_USER,
    CMD_JOIN,
    CMD_PART,
    CMD_PRIVMSG,
    CMD_KICK,
    CMD_INVITE,
    CMD_TOPIC,
    CMD_MODE,
    CMD_QUIT,
    CMD_PING,
    CMD_CAP
};

class Client
{
private:
    int                 _fd;
    std::string         _nickname;
    std::string         _username;
    std::string         _realname;
    std::string         _hostname;
    std::string         _buffer;
    bool                _passOk;
    bool                _nickSet;
    bool                _userSet;
    bool                _registered;
    std::set<std::string> _channels;

public:
    Client();
    Client(int fd);
    Client(const Client &other);
    Client &operator=(const Client &other);
    ~Client();

    int                 getFd() const;
    std::string         getNickname() const;
    std::string         getUsername() const;
    std::string         getRealname() const;
    std::string         getHostname() const;
    std::string&        getBuffer();
    bool                isPassOk() const;
    bool                isNickSet() const;
    bool                isUserSet() const;
    bool                isRegistered() const;
    std::string         getPrefix() const;
    const std::set<std::string>& getChannels() const;

    void                setFd(int fd);
    void                setNickname(const std::string &nickname);
    void                setUsername(const std::string &username);
    void                setRealname(const std::string &realname);
    void                setHostname(const std::string &hostname);
    void                setPassOk(bool ok);
    void                setNickSet(bool set);
    void                setUserSet(bool set);
    void                setRegistered(bool registered);

    void                joinChannel(const std::string &channel);
    void                leaveChannel(const std::string &channel);
    bool                isInChannel(const std::string &channel) const;

    void                appendToBuffer(const std::string &data);
    void                clearBuffer();
    bool                extractLine(std::string &line);

    void                sendMessage(const std::string &message) const;
    void                sendReply(const std::string &serverName, const std::string &code, const std::string &message) const;

    bool                canRegister() const;
    bool                tryRegister();

    static bool         isValidNickname(const std::string &nickname);
    static std::vector<std::string> split(const std::string &str, char delim);
    static void         parseIRCLine(const std::string &line, std::string &command, std::vector<std::string> &params);
    static CommandType  getCommandType(const std::string &command);
    static bool         requiresRegistration(CommandType type);
};