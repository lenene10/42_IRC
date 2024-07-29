#pragma once

#include <string>
#include <vector>
#include "Channel.hpp"
#include "Network.hpp"



class Channel;

class User
{
	private:
		int id;
		bool loged;
		std::string password;
		std::string nickname;
		std::string username;
		std::vector<Channel*> channels;

	public:
		User(int id);
		~User();

		int getID();
		bool isLoged();
		void setLoged(bool status);
		std::string getPassword();
		void setPassword(std::string pw);
		std::string getNickname();
		void setNickname(std::string nn);
		std::string getUsername();
		void setUsername(std::string un);
		Channel& findChannel(std::string name);
		void addChannel(Channel* ch);
		void leaveChannel(std::string name);
		bool inChannel(std::string name);
		void disconnect(Network& nw);

		bool isAuthenticated();
};
