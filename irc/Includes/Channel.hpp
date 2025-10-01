#pragma once
#include <Server.hpp>
#include <set>
#include <string>
#include <vector>
class Channel
{
  private:
    // channel attributes
    std::string _username;
    std::string _nickname;
    std::string topic;
    std::string password;
    std::set<int> members;          // fds of each client;
    std::set<std::string> _invited; /// invited peoples nickname
    // channel settings

    bool invite_only;
    bool topic_restricted;
    int user_limit;

  public:
    // Getters
    const Channel &getName() const
    {
        return (Channel());
    }
    const std::string &getTopic() const
    {
        return (topic);
    }
    const std::string &getpassword() const
    {
        return (password);
    }
    size_t getMemberCount() const
    {
        return (members.size());
    }
    bool isInviteOnly() const
    {
        return (invite_only);
    }
    bool hasUserLimit() const
    {
        return (user_limit > 0);
    }
    int getUserLimit() const
    {
        return (user_limit);
    }

    // Setters
    void setKey(const std::string &pass)
    {
        this->password = pass;
    }
    void setInviteOnly(bool mode)
    {
        invite_only = mode;
    }
    void setUserLimit(int limit)
    {
        user_limit = limit;
    }

    //  operations
    // std::vector<std::string> ft_split_request(std::string &request, const std::string &delimiter);
    void addMember(int client_fd, const std::string& nick);
    void removeMember(int client_fd);
    bool isMember(int client_fd);
    bool isOperator(int client_fd);
};