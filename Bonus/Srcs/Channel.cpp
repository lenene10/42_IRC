#include "../Incl/Channel.hpp"

Channel::Channel() : topic(""), maxUsers(0), inviteOnly(false), privateTopic(false), password(""), name(""), botEnabled(true) {}

Channel::Channel(std::string name) : topic(""), maxUsers(0), inviteOnly(false), privateTopic(false), password(""), name(name), botEnabled(true) {}

Channel::~Channel() {}

void Channel::addUser(User& user) {
	if (this->invited.find(user.getID()) != this->invited.end())
		this->invited.erase(this->invited.find(user.getID()));
	else if (this->inviteOnly)
		throw std::runtime_error("Error: this Channel is invite Only\n");

	if (this->users.find(user.getID()) == this->users.end())
	{
		user.addChannel(this);
		this->users[user.getID()] = 0;
	}
}

void Channel::addAdmin(User& user) {
	this->addUser(user);

	if (this->admins.find(user.getID()) == this->admins.end())
		this->admins[user.getID()] = true;
}

void Channel::addAnotherAdmin(User& user)
{
	if (this->admins.find(user.getID()) == this->admins.end())
		this->admins[user.getID()] = true;
}

void Channel::removeUser(Network& nw, User& user) {
		this->removeAdmin(nw, user);

	user.leaveChannel(this->name);
	if (this->users.find(user.getID()) != this->users.end())
		this->users.erase(this->users.find(user.getID()));
}

void Channel::removeAdmin(Network& nw, User& user) {
	if (this->admins.find(user.getID()) != this->admins.end())
		this->admins.erase(this->admins.find(user.getID()));

	if (this->users.size() != 1 && this->admins.size() == 0) {
		std::map<int, int>::iterator it = this->users.find(user.getID());
		it++;
		if (it == this->users.end())
		{
			it--;
			it--;
		}
		this->admins[it->first] = true;
		User& us = nw.getUser(it->first);
		this->sendMsg(nw, us, "has been promoted to Admin");
		nw.sendMsg(us.getID(), this->name + "> you have been promoted to Admin\n");
	}
}

bool Channel::isAdmin(User& user) {
	if (this->admins.find(user.getID()) != this->admins.end())
		return (true);
	return (false);
}
		
void Channel::invite(User& user) {
	if (this->invited.find(user.getID()) == this->invited.end())
		this->invited[user.getID()] = 0;
}

bool Channel::isInvited(User& user) {
	if (this->invited.find(user.getID()) != this->invited.end())
		return (true);
	return (false);
}

bool Channel::isFull() {
	if (this->maxUsers == 0)
		return (false);
	else if (this->users.size() < this->maxUsers)
		return (false);
	return (true);
}

std::string& Channel::getChannelName() {
	return (this->name);
}

User& Channel::findUser(const std::string& str, Network& nw) {
	for (std::map<int, int>::iterator it = this->users.begin(); it != this->users.end(); it++) {
		User& us = nw.getUser(it->first);
			
		if (us.getUsername() == str)
			return (us);
	}
	throw std::runtime_error("Error: User not found!\n");
}

void Channel::setMaxUsers(int max) {
	this->maxUsers = max;
}

int Channel::getMaxUsers() {
	return (this->maxUsers);
}

void Channel::toggelInviteOnly() {
	this->inviteOnly = !this->inviteOnly;
}

void Channel::toggleBot()
{
	if (botEnabled)
		botEnabled = false;
	else
		botEnabled = true;
}

bool Channel::getBotEnabled()
{
	return (this->botEnabled);
}

bool Channel::getInviteOnly() {
	return (this->inviteOnly);
}

void Channel::toggelPrivateTopic() {
	this->privateTopic = !this->privateTopic;
}

bool Channel::getPrivateTopic() {
	return (this->privateTopic);
}

void Channel::setPassword(std::string& password) {
	this->password = password;
}

const std::string& Channel::getPassword() {
	return (this->password);
}

void Channel::setTopic(std::string& topic) {
	this->topic = topic;
}

int Channel::getUserCount() {
	return (this->users.size());
}

std::string& Channel::getTopic() {
	return (this->topic);
}

bool Channel::alreadyJoined(User& user) {
	if (this->users.find(user.getID()) != this->users.end())
		return (true);
	return (false);
}

void Channel::sendMsg(Network& nw, User& user, std::string msg)
{
	//the user variable here is the sender user!!
	for (std::map<int, int>::iterator it = this->users.begin(); it != this->users.end(); ++it)
	{
		User& us = nw.getUser(it->first);

		if (us.getID() != user.getID() && us.getID() != 0) // If the user is not the sender and not the Bot
			//sent message to the channel, actually we are just sending to each user in the channels' user vector
			nw.sendMsg(us.getID(), this->name + "> " + user.getUsername() + "> " + msg + "\n");
	}
}