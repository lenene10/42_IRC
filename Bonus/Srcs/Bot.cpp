#include "../Incl/Bot.hpp"
#include <iostream>
#include <sstream>

Bot::Bot() : User(0) {
}

Bot::~Bot() {}

bool Bot::receiveAndProcessMessages(Network& nw, std::vector<User>& users, int index) {
	std::istringstream iss(nw.getCach(index));
	std::string command;
	iss >> command;

	if (command == "/HELP") {
		help(nw, users[index - 1]);
		return true;
	}
	return false;
}

std::string toLower(std::string str) {
	for (unsigned int i = 0; i < str.length(); i++) {
		str[i] = tolower(str[i]);
	}
	return str;
}

void Bot::replaceBadWords(std::string& str) {
	std::string badWords[] = {"fuck", "bitch", "ass", "dick"};
	std::string str_lowercase = toLower(str);
	for (int i = 0; i < 4; i++) {
		size_t found = str_lowercase.find(badWords[i]);
		while (found != std::string::npos) {
			str.replace(found, badWords[i].length(), "****");
			str_lowercase.replace(found, badWords[i].length(), "****");
			found = str_lowercase.find(badWords[i]);
		}
	}
}

void Bot::help(Network& nw, User& us) {

	nw.sendMsg(us.getID(), "Available commands:\n");
	nw.sendMsg(us.getID(), "PASS <password>\t\t\t - Set password\n");
	nw.sendMsg(us.getID(), "NICK <nick>\t\t\t - Set nick\n");
	nw.sendMsg(us.getID(), "USER <user>\t\t\t- Set user information\n");
	nw.sendMsg(us.getID(), "JOIN <#channel> <password>\t - Join a channel\n");
	nw.sendMsg(us.getID(), "LEAVE <#channel>\t\t - Leave a channel\n");
	nw.sendMsg(us.getID(), "KICK <#channel> <user>\t\t - Kick a user from a channel\n");
	nw.sendMsg(us.getID(), "INVITE <user> <#channel>\t - Invite a user to a channel\n");
	nw.sendMsg(us.getID(), "TOPIC <#channel> <topic>\t - Set or get the topic of a channel\n");
	nw.sendMsg(us.getID(), "MODE <#channel> <option>\t - Set channel or user modes\n");
	nw.sendMsg(us.getID(), "PRIVMSG <user or #channel> : <message>\t - Send a private message\n");
}