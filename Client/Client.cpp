#include "Client.hpp"
#include <unistd.h>
#include <cctype>
#include <sstream>

Client::Client() : _fd(-1), _passOk(false), _nickSet(false), _userSet(false), _registered(false) {}

Client::Client(int fd) : _fd(fd), _passOk(false), _nickSet(false), _userSet(false), _registered(false) {}

Client::Client(const Client &other) {
    *this = other;
}

Client &Client::operator=(const Client &other) {
    if (this != &other) {
        _fd = other._fd;
        _nickname = other._nickname;
        _username = other._username;
        _realname = other._realname;
        _hostname = other._hostname;
        _buffer = other._buffer;
        _passOk = other._passOk;
        _nickSet = other._nickSet;
        _userSet = other._userSet;
        _registered = other._registered;
        _channels = other._channels;
    }
    return *this;
}

Client::~Client() {}

int Client::getFd() const { return _fd; }
std::string Client::getNickname() const { return _nickname; }
std::string Client::getUsername() const { return _username; }
std::string Client::getRealname() const { return _realname; }
std::string Client::getHostname() const { return _hostname; }
std::string& Client::getBuffer() { return _buffer; }
bool Client::isPassOk() const { return _passOk; }
bool Client::isNickSet() const { return _nickSet; }
bool Client::isUserSet() const { return _userSet; }
bool Client::isRegistered() const { return _registered; }
const std::set<std::string>& Client::getChannels() const { return _channels; }

std::string Client::getPrefix() const {
    return _nickname + "!" + _username + "@" + _hostname;
}

void Client::setFd(int fd) { _fd = fd; }
void Client::setNickname(const std::string &nickname) { _nickname = nickname; }
void Client::setUsername(const std::string &username) { _username = username; }
void Client::setRealname(const std::string &realname) { _realname = realname; }
void Client::setHostname(const std::string &hostname) { _hostname = hostname; }
void Client::setPassOk(bool ok) { _passOk = ok; }
void Client::setNickSet(bool set) { _nickSet = set; }
void Client::setUserSet(bool set) { _userSet = set; }
void Client::setRegistered(bool registered) { _registered = registered; }

void Client::joinChannel(const std::string &channel) { _channels.insert(channel); }
void Client::leaveChannel(const std::string &channel) { _channels.erase(channel); }
bool Client::isInChannel(const std::string &channel) const { 
    return _channels.find(channel) != _channels.end(); 
}

void Client::appendToBuffer(const std::string &data) { _buffer += data; }
void Client::clearBuffer() { _buffer.clear(); }

bool Client::extractLine(std::string &line) {
    size_t pos = _buffer.find('\n');
    if (pos == std::string::npos)
        return false;
    
    line = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 1);
    
    if (!line.empty() && line[line.length() - 1] == '\r')
        line.erase(line.length() - 1);
    
    return true;
}

void Client::sendMessage(const std::string &message) const {
    std::string msg = message + "\r\n";
    send(_fd, msg.c_str(), msg.length(), 0);
}

void Client::sendReply(const std::string &serverName, const std::string &code, const std::string &message) const {
    std::string nick = _nickname.empty() ? "*" : _nickname;
    std::string reply = ":" + serverName + " " + code + " " + nick + " " + message;
    sendMessage(reply);
}

bool Client::canRegister() const {
    return _passOk && _nickSet && _userSet && !_registered;
}

bool Client::tryRegister() {
    if (canRegister()) {
        _registered = true;
        return true;
    }
    return false;
}

bool Client::isValidNickname(const std::string &nickname) {
    if (nickname.empty() || nickname.length() > 9)
        return false;
    if (!isalpha(nickname[0]) && nickname[0] != '_')
        return false;
    for (size_t i = 1; i < nickname.length(); i++) {
        if (!isalnum(nickname[i]) && nickname[i] != '_' && nickname[i] != '-')
            return false;
    }
    return true;
}

std::vector<std::string> Client::split(const std::string &str, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        if (!token.empty())
            tokens.push_back(token);
    }
    return tokens;
}

void Client::parseIRCLine(const std::string &line, std::string &command, std::vector<std::string> &params) {
    command.clear();
    params.clear();
    
    if (line.empty())
        return;
    
    size_t pos = 0;
    size_t start = 0;
    
    while (pos < line.length() && line[pos] == ' ')
        pos++;
    start = pos;
    
    while (pos < line.length() && line[pos] != ' ')
        pos++;
    command = line.substr(start, pos - start);
    
    for (size_t i = 0; i < command.length(); i++)
        command[i] = toupper(command[i]);
    
    while (pos < line.length()) {
        while (pos < line.length() && line[pos] == ' ')
            pos++;
        if (pos >= line.length())
            break;
        
        if (line[pos] == ':') {
            params.push_back(line.substr(pos + 1));
            break;
        }
        
        start = pos;
        while (pos < line.length() && line[pos] != ' ')
            pos++;
        params.push_back(line.substr(start, pos - start));
    }
}

CommandType Client::getCommandType(const std::string &command) {
    if (command == "PASS")    return CMD_PASS;
    if (command == "NICK")    return CMD_NICK;
    if (command == "USER")    return CMD_USER;
    if (command == "JOIN")    return CMD_JOIN;
    if (command == "PART")    return CMD_PART;
    if (command == "PRIVMSG") return CMD_PRIVMSG;
    if (command == "KICK")    return CMD_KICK;
    if (command == "INVITE")  return CMD_INVITE;
    if (command == "TOPIC")   return CMD_TOPIC;
    if (command == "MODE")    return CMD_MODE;
    if (command == "QUIT")    return CMD_QUIT;
    if (command == "PING")    return CMD_PING;
    if (command == "CAP")     return CMD_CAP;
    return CMD_UNKNOWN;
}

bool Client::requiresRegistration(CommandType type) {
    return type != CMD_PASS && type != CMD_NICK && 
           type != CMD_USER && type != CMD_QUIT && type != CMD_CAP;
}