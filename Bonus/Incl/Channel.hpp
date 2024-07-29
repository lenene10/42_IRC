#pragma once

#include <string>
#include <vector>
#include <map>
#include "User.hpp"
#include "Network.hpp"

class User;
class Network;

class Channel
{
	private:
		std::string topic;
		std::map<int, int> users;
		std::map<int, bool> admins;
		std::map<int, int> invited;
		unsigned int maxUsers;
		bool inviteOnly;
		bool privateTopic;
		std::string password;
		std::string name;
		bool botEnabled;
		
	public:
		Channel();
		Channel(std::string name);
		~Channel();

		void addUser(User& user);
		void addAdmin(User& user);
		void addAnotherAdmin(User& user);
		void removeUser(Network& nw, User& user);
		void removeAdmin(Network& nw, User& user);
		bool isAdmin(User& user);
		void invite(User& user);
		bool isInvited(User& user);
		bool isFull();

		std::string& getChannelName();
		User& findUser(const std::string& str, Network& nw);
		void setMaxUsers(int max);
		int getMaxUsers();
		void toggelInviteOnly();
		bool getInviteOnly();
		void toggelPrivateTopic();
		bool getPrivateTopic();
		void setPassword(std::string& password);
		const std::string& getPassword();
		void setTopic(std::string& topic);
		std::string& getTopic();
		int getUserCount();
		bool alreadyJoined(User& user);

		void toggleBot();
		bool getBotEnabled();
		
		void sendMsg(Network& nw, User& user, std::string msg);
};
