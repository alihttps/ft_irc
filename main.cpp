#include "Server.hpp"

int main(int argc, char **argv)
{
    if(argc == 3)
    {
        try
        {
            Server::GetArgsToParse(argv);
        }
    }
    std::cout << argv[0] << " <port> <password>" << std::endl;
    return (127);
}