#include <Channel.hpp>

void Channel::setKey(const std::string &pass)
{
	this->password = pass;
}
void Channel::addMember(int client_fd)
{
	this->members.insert(client_fd);
}

void Channel::removeMember(int client_fd)
{
	this->members.erase(client_fd);
}

bool Channel::isMember(int client_fd)
{
	return (this->members.find(client_fd) != this->members.end());
}

bool Channel::isOperator(int /*client_fd*/)
{
	// TODO: No operators structure exists yet. Add e.g.,
		std::set<int> operators;
	// and implement accordingly.
	return (false);
}
