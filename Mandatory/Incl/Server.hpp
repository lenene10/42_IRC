#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>

#include "User.hpp"
#include "Channel.hpp"
#include "Network.hpp"
#include "Parse.hpp"

class Server
{
	private:
		std::string password;
		std::vector<User> users;
		std::map<std::string, Channel> channels;
		Network nw;

	public:
		Server(unsigned short port, std::string password);
		~Server();

		void run();

	private:
		void checkChannels();
		bool matchCommands(const std::string& str, int index);
		bool doSendFile(std::istringstream& iss, int index);
		bool doRecvFile(std::istringstream& iss, int index);
		bool doMsg(std::istringstream& iss, int index);
		bool doPrivmsg(std::istringstream& iss, int index);
		bool doPass(std::istringstream& iss, int index);
		bool doNick(std::istringstream& iss, int index);
		bool doUser(std::istringstream& iss, int index);
		bool doJoin(std::istringstream& iss, int index);
		bool doLeave(std::istringstream& iss, int index);
		bool doKick(std::istringstream& iss, int index);
		bool doInvite(std::istringstream& iss, int index);
		bool doTopic(std::istringstream& iss, int index);
		bool doMode(std::istringstream& iss, int index);

};

#endif
