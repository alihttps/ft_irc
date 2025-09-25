#include "Server.hpp"
#include "Client.hpp"
#include <fcntl.h>

int main(int argc, char **argv)
{
    if(argc == 3)
    {
        try
        {
            Server::GetArgsToParse(const_cast<const char **>(argv));
            // to test
            {
                int fd = open("test", O_RDWR | O_CREAT, 0666);
                Client client(fd, argv[2]);
                client.ClientInvoke();
            }
            std::cout << "Server starting with port " << argv[1] << std::endl;
            return (0);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return (1);
        }
    }
    std::cout << "Usage: " << argv[0] << " <port> <password>" << std::endl;
    return (127);
}