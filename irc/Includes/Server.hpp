#pragma once
#include <set>
#include <map>
#include <arpa/inet.h>
#include <cstring>
#include <deque>
#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

class Server
{
  private:
	struct addrinfo hints, *ai, *p;
	std::vector<struct pollfd> pfds;
	int fd_count;
	int listener;
	void add_to_pfds(const int &newfd);
	void del_from_pfds(const int &i);
	void handle_new_connection();
	void handle_client(const int &i);
	int set_listening_socket();
	void process_connections();
	static void GetArgsToParse(const char **const argv);
	Server();

  public:
	static Server &get_server()
	{
		static Server server;
		return (server);
	}
	void server_init();
	~Server();
};
