#pragma once
#include "Server.hpp"

class Client
{
  private:
	int Fd;
	std::string Ipaddress;
	// Private umembers will go here

  public:
      Client();
      ~Client();
	void set_fd(int fd)
	{
		this->Fd = fd;
	}
	int get_fd()
	{
		return (this->Fd);
	}
	void set_ipadd(std::string add)
	{
		this->Ipaddress = add;
	}
	std::string get_ipadd()
	{
		return (this->Ipaddress);
	}
};