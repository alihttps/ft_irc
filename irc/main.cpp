#include "Server.hpp"
#include "Client.hpp"

int	main(int argc, char **argv)
{
	if (argc == 3)
	{
		try
		{
			Server::get_server().server_init();
			// Server::GetArgsToParse(const_cast<const char **>(argv));
			std::cout << "Server starting with port " << argv[1] << std::endl;
			return (0);
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
			return (1);
		}
	}
	std::cout << "Usage: " << argv[0] << " <port> <password>" << std::endl;
	return (127);
}