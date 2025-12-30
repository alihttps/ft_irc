#include "Server.hpp"

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    if (argc == 3)
    {
        try
        {
            std::cout << "Server starting with port " << argv[1] << std::endl;
            Server::getInstance().run(argv[1], argv[2]);
            return 0;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    std::cout << "Usage: " << argv[0] << " <port> <password>" << std::endl;
    return 1;
}