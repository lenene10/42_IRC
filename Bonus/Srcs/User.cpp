#include "../Incl/User.hpp"

User::User(int id) : id(id), loged(false), password(""), nickname(""), username("") {}


User::~User() {}

int User::getID() {
	return (this->id);
}

bool User::isLoged() {
	return (this->loged);
}

void User::setLoged(bool status) {
	this->loged = status;
}

std::string User::getPassword() {
	return (this->password);
}

void User::setPassword(std::string pw) {
	this->password = pw;
}

std::string User::getNickname() {
	return (this->nickname);
}

void User::setNickname(std::string nn) {
	this->nickname = nn;
}

std::string User::getUsername() {
	return (this->username);
}

void User::setUsername(std::string un) {
	this->username = un;
}

Channel& User::findChannel(std::string name) {
	for (size_t i = 0; i < this->channels.size(); i++) {
		Channel* ch = this->channels[i];
		if (ch->getChannelName() == name)
			return (*ch);
	}
	throw std::runtime_error("Error: You are not in this Channel!\n");
}

void User::addChannel(Channel* ch) {
	this->channels.push_back(ch);
}

void User::leaveChannel(std::string name) {
	for (size_t i = 0; i < this->channels.size(); i++)
	{
		Channel* ch = this->channels[i];
		if (ch && ch->getChannelName() == name)
		{
			this->channels.erase(this->channels.begin() + i);
			return ;
		}
	}
	throw std::runtime_error("Error: Channel not found!\n");
}

bool User::inChannel(std::string name) {
	for (size_t i = 0; i < this->channels.size(); i++) {
		Channel* ch = this->channels[i];
		if (ch && ch->getChannelName() == name)
			return (true);
	}
	return (false);
}

void User::disconnect(Network& nw) {
	for (size_t i = 0; i < this->channels.size(); i++) {
		Channel* ch = this->channels[i];
		if (ch != 0) {
			ch->removeUser(nw, *this);
		}
	}
}

bool User::isAuthenticated() {
	if (this->password.size() != 0 && this->username.size() != 0 && this->nickname.size() != 0)
		return (true);
	return (false);
}
