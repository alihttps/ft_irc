#include <Channel.hpp>

// JOIN command implementation
// This file will contain the JOIN command logic for IRC
std::vector<std::string> ft_split_request(std::string &request,
	const std::string &delimiter)
{
	size_t	end;
	size_t	start;
	int		i;

	std::vector<std::string> lines;
	start = 0;
	i = 0;
	end = request.find(delimiter);
	while (end != std::string::npos)
	{
		lines.push_back(request.substr(start, end - start));
		// std::cout << "testing line number " << i << " :" << lines[i] << std::endl;
		start = end + delimiter.length();
		end = request.find(delimiter, start);
		i++;
	}
	lines.push_back(request.substr(start));
	return (lines);
}
int	check_channel_name(std::string &c_name)
{
	if (c_name.length() > 1)
	{
		if (c_name[0] == '#')
			return (1);
	}
	return (0);
}
int	check_allchannels_name(std::vector<std::string> &channels)
{
	for (int i = 0; i < channels.size(); i++)
	{
		if(check_channel_name(channels[i]) == 0)
		return i; // returning which on is not valid
	}
	return 0; //succes
}
int	count_char_in_string(std::string &stringg, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (i < stringg.length())
	{
		if (stringg[i] == c)
			count++;
		i++;
	}
	return (count);
}

int	valid_syntax(std::string &request)
{
	std::vector<std::string> divided = ft_split_request(request, " ");
	if (divided.size() < 2)
		throw("error not enough parametres \n");
	if (divided[0] == "JOIN")
	{
		
	}
}
// void	addMember(int client_fd, const std::string &nick)
// {

// }
// void	removeMember(int client_fd)
// {
// }
// bool	isMember(int client_fd)
// {
// }
// bool	isOperator(int client_fd)
// {
// }