#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <poll.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <vector>
#include "User.hpp"
#include "Channel.hpp"

class Channel;
class User;

class Network
{
	private:
		int connections;
		std::vector<struct pollfd> fds;
		struct sockaddr_in addr;
		std::vector<User>& users;
		std::vector<std::string> cach;
	
	public:
		Network(unsigned short port, std::vector<User>& users);
		~Network();
		
		int recvMsg(int index);
		void sendMsg(int id, const std::string& msg);
		std::string& getCach(int index);
		void clearCach(int index);

		void acceptClients();

		int getIndex(int id);
		User& getUser(int id);
		User& findUser(std::string& name);
		struct pollfd* getFDs();
		int getConnections();

		void disconnectClient(int index);
};

#endif