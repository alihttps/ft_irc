#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

class Client;

class Channel
{
private:
    std::string                 _name;
    std::string                 _topic;
    std::string                 _key;
    std::set<int>               _members;
    std::set<int>               _operators;
    std::set<std::string>       _inviteList;
    bool                        _inviteOnly;
    bool                        _topicRestricted;
    size_t                      _userLimit;

public:
    Channel();
    Channel(const std::string &name);
    Channel(const Channel &other);
    Channel &operator=(const Channel &other);
    ~Channel();

    std::string             getName() const;
    std::string             getTopic() const;
    std::string             getKey() const;
    const std::set<int>&    getMembers() const;
    const std::set<int>&    getOperators() const;
    bool                    isInviteOnly() const;
    bool                    isTopicRestricted() const;
    size_t                  getUserLimit() const;
    size_t                  getMemberCount() const;

    void                    setName(const std::string &name);
    void                    setTopic(const std::string &topic);
    void                    setKey(const std::string &key);
    void                    setInviteOnly(bool inviteOnly);
    void                    setTopicRestricted(bool topicRestricted);
    void                    setUserLimit(size_t limit);

    void                    addMember(int fd);
    void                    removeMember(int fd);
    bool                    isMember(int fd) const;
    bool                    isEmpty() const;

    void                    addOperator(int fd);
    void                    removeOperator(int fd);
    bool                    isOperator(int fd) const;

    void                    addInvite(const std::string &nickname);
    void                    removeInvite(const std::string &nickname);
    bool                    isInvited(const std::string &nickname) const;

    void                    broadcast(const std::string &message, int excludeFd = -1);

    std::string             getModeString() const;
};
