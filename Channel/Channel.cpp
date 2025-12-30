#include "Channel.hpp"
#include "Server.hpp"

Channel::Channel() : _inviteOnly(false), _topicRestricted(false), _userLimit(0) {}

Channel::Channel(const std::string &name) : _name(name), _inviteOnly(false), _topicRestricted(false), _userLimit(0) {}

Channel::Channel(const Channel &other) {
    *this = other;
}

Channel &Channel::operator=(const Channel &other) {
    if (this != &other) {
        _name = other._name;
        _topic = other._topic;
        _key = other._key;
        _members = other._members;
        _operators = other._operators;
        _inviteList = other._inviteList;
        _inviteOnly = other._inviteOnly;
        _topicRestricted = other._topicRestricted;
        _userLimit = other._userLimit;
    }
    return *this;
}

Channel::~Channel() {}

std::string Channel::getName() const { return _name; }
std::string Channel::getTopic() const { return _topic; }
std::string Channel::getKey() const { return _key; }
const std::set<int>& Channel::getMembers() const { return _members; }
const std::set<int>& Channel::getOperators() const { return _operators; }
bool Channel::isInviteOnly() const { return _inviteOnly; }
bool Channel::isTopicRestricted() const { return _topicRestricted; }
size_t Channel::getUserLimit() const { return _userLimit; }
size_t Channel::getMemberCount() const { return _members.size(); }

void Channel::setName(const std::string &name) { _name = name; }
void Channel::setTopic(const std::string &topic) { _topic = topic; }
void Channel::setKey(const std::string &key) { _key = key; }
void Channel::setInviteOnly(bool inviteOnly) { _inviteOnly = inviteOnly; }
void Channel::setTopicRestricted(bool topicRestricted) { _topicRestricted = topicRestricted; }
void Channel::setUserLimit(size_t limit) { _userLimit = limit; }

void Channel::addMember(int fd) { _members.insert(fd); }
void Channel::removeMember(int fd) { 
    _members.erase(fd);
    _operators.erase(fd);
}
bool Channel::isMember(int fd) const { return _members.find(fd) != _members.end(); }
bool Channel::isEmpty() const { return _members.empty(); }

void Channel::addOperator(int fd) { _operators.insert(fd); }
void Channel::removeOperator(int fd) { _operators.erase(fd); }
bool Channel::isOperator(int fd) const { return _operators.find(fd) != _operators.end(); }

void Channel::addInvite(const std::string &nickname) { _inviteList.insert(nickname); }
void Channel::removeInvite(const std::string &nickname) { _inviteList.erase(nickname); }
bool Channel::isInvited(const std::string &nickname) const { 
    return _inviteList.find(nickname) != _inviteList.end(); 
}

void Channel::broadcast(const std::string &message, int excludeFd) {
    for (std::set<int>::iterator it = _members.begin(); it != _members.end(); ++it) {
        if (*it != excludeFd) {
            Client *client = Server::getInstance().getClient(*it);
            if (client)
                client->sendMessage(message);
        }
    }
}

std::string Channel::getModeString() const {
    std::string modes = "+";
    std::string params = "";
    
    if (_inviteOnly)
        modes += "i";
    if (_topicRestricted)
        modes += "t";
    if (!_key.empty()) {
        modes += "k";
        params += " " + _key;
    }
    if (_userLimit > 0) {
        modes += "l";
        std::ostringstream oss;
        oss << _userLimit;
        params += " " + oss.str();
    }
    
    if (modes == "+")
        return "";
    return modes + params;
}
