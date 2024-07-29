#include "../Incl/Server.hpp"

Server::Server(unsigned short port, std::string password) : password(password), nw(port, this->users) {}

Server::~Server() {}

void Server::run()
{
	std::cout << "Server Started" << std::endl;

	while (1)
	{
		poll(this->nw.getFDs(), this->nw.getConnections(), -1);

		this->nw.acceptClients();

		for (int i = 1; i < this->nw.getConnections(); i++)
		{
			if (!(this->nw.getFDs()[i].revents & POLLIN))
				continue;

			int status = this->nw.recvMsg(i);
			if (status)
			{
				if (status == 2) // if a client disconnects
				{
					i--;
					checkChannels();
				}
				continue;
			}
			
			this->matchCommands(nw.getCach(i), i);
			nw.clearCach(i);
		}
	}
}

void Server::checkChannels()
{
	for (std::map<std::string, Channel>::iterator it = this->channels.begin(); it != this->channels.end();)
	{
		std::map<std::string, Channel>::iterator itnew = it;
		it++;
		if (itnew->second.getUserCount() == 0)
			this->channels.erase(itnew);
	}
}

bool findColon(std::istringstream& iss)
{
	char a;
	while (iss.get(a))
	{
		if (a == ':')
			return (true);
		else if (a != ' ' && a != '\t')
			return (false);
	}
	return (false);
}


static std::string getNextWord(std::istringstream& iss) {
    std::string word;
    iss >> word;
    return (word);
}

bool Server::matchCommands(const std::string& str, int index)
{
	const int size = 10;
	char commands[size][10] = {"PASS", "NICK", "USER", "JOIN", "LEAVE", "KICK", "INVITE", "TOPIC", "MODE", "PRIVMSG"};
	bool (Server::*funcs[])(std::istringstream& iss, int index) = 
			{&Server::doPass, &Server::doNick, &Server::doUser, &Server::doJoin, &Server::doLeave, 
			&Server::doKick, &Server::doInvite, &Server::doTopic, &Server::doMode, &Server::doPrivmsg};
    std::istringstream iss(str);
    std::string firstword = getNextWord(iss);

	for (size_t i = 0; i < size; i++)
	{
		if (commands[i] == firstword)
		{
			try
			{
				return ((this->*funcs[i])(iss, index));
			}
			catch (std::exception& e)
			{
				this->nw.sendMsg(this->users[index - 1].getID(), e.what());
				return (true);
			}
		}
	}
	this->nw.sendMsg(this->users[index - 1].getID(), "Error: Command not Found!\n");

	return (false);
}

bool Server::doPrivmsg(std::istringstream& iss, int index)
{
	if (!this->users[index - 1].isLoged())
		throw std::runtime_error("Error: you are not authenticated!\nUse PASS, NICK and USER to authenticate!\n");
	std::string name = getNextWord(iss);

	if (name.size() == 0 || name[0] == ':')
		throw std::runtime_error("Error: expected a name\n");
	if (!findColon(iss))
		throw std::runtime_error("Error: Message start does not exist!\n");
	
	std::string data;
	std::getline(iss, data);
	if (name[0] == '#' || name[0] == '&')
	{
		Channel& ch = this->users[index - 1].findChannel(name);
		ch.sendMsg(this->nw, this->users[index - 1], data);
	}
	else
	{
		User& user = this->nw.findUser(name);

		this->nw.sendMsg(user.getID(), "PRIVMSG> " + this->users[index - 1].getUsername() + "> " + data + "\n");
	}
	return (0);
}

bool Server::doPass(std::istringstream& iss, int index)
{
	std::string pass = getNextWord(iss);
	User& user = this->users[index - 1];

	if (user.getPassword().size() != 0)
		throw std::runtime_error("Error: you already set the Server Password!\n");

	if (pass == this->password)
	{
		user.setPassword(pass);
		this->nw.sendMsg(user.getID(), "Correct Password!\n");
	}
	else
		throw std::runtime_error("Wrong Password!\n");
	if (!user.isLoged() && user.isAuthenticated())
	{
		this->nw.sendMsg(user.getID(), "Welcome to the Server!\n");
		user.setLoged(true);
	}
	return (true);
}

bool Server::doNick(std::istringstream& iss, int index)
{
	std::string nick = getNextWord(iss);
	User& user = this->users[index - 1];

	if (nick.size() == 0)
		throw std::runtime_error("Error: you have to specify a nickname!\n");
	user.setNickname(nick);
	this->nw.sendMsg(user.getID(), "Your nickname is set to '" + nick + "'\n");

	if (!user.isLoged() && user.isAuthenticated())
	{
		this->nw.sendMsg(user.getID(), "Welcome to the Server!\n");
		user.setLoged(true);
	}
	return (true);
}

bool isStrAlnum(const std::string& str) {
	for (size_t i = 0; i < str.size(); i++)
		if (!isalnum(str[i]))
			return (false);
	return (true);
}
bool Server::doUser(std::istringstream& iss, int index)
{
	std::string username = getNextWord(iss);
	User& user = this->users[index - 1];

	if (username.size() == 0)
		throw std::runtime_error("Error: You have to specify a Username!\n");
	if (isStrAlnum(username) == false)
		throw std::runtime_error("Error: Username must be alphanumeric!\n");

	for (size_t i = 0; i < this->users.size(); i++)
		if (this->users[i].getUsername() == username)
			throw std::runtime_error("Error: This username is already taken!\n");
	user.setUsername(username);
	this->nw.sendMsg(user.getID(), "You have set your Username to '" + username + "'\n");

	if (!user.isLoged() && user.isAuthenticated())
	{
		this->nw.sendMsg(user.getID(), "Welcome to the Server!\n");
		user.setLoged(true);
	}
	return (true);
}

bool Server::doJoin(std::istringstream& iss, int index)
{
	if (!this->users[index - 1].isLoged())
		throw std::runtime_error("Error: you are not authenticated!\nUse PASS, NICK and USER to authenticate!\n");
	std::string channelname = getNextWord(iss);
	std::string password = getNextWord(iss);
	

	if (this->users[index - 1].inChannel(channelname))
		throw std::runtime_error("Error: You are already in this channel!\n");

	if (this->channels.find(channelname) != this->channels.end())
	{
		Channel& ch = this->channels.find(channelname)->second;
		if (ch.isFull())
			throw std::runtime_error("Error: This server is already full\n");
		if (!ch.isInvited(this->users[index - 1]) && ch.getPassword().size() != 0 && ch.getPassword() != password)
		{
			if (password.size() != 0 && ch.getPassword() != password)
				throw std::runtime_error("Error: Wrong Password!\n");
			throw std::runtime_error("This Channel is password protected. Please enter a password!\n");
		}

		ch.addUser(this->users[index - 1]);
		this->nw.sendMsg(this->users[index - 1].getID(), "You joined the Channel " + channelname + "\n");
		this->nw.sendMsg(this->users[index - 1].getID(), "The channels topic is: " + ch.getTopic() + "\n");
		ch.sendMsg(this->nw, this->users[index - 1], "Joined");
	}
	else
	{
		if ((channelname[0] != '#' && channelname[0] != '&') || (channelname.size() > 200 || channelname.size() < 2))
			throw std::runtime_error("Error: Channel name has to start with # or &\n");
		Channel ch(channelname);
		this->channels[channelname] = ch;
		this->channels[channelname].addAdmin(this->users[index - 1]);
		this->nw.sendMsg(this->users[index - 1].getID(), "You created the Channel " + channelname + "\n");
	}
	return (true);
}

bool Server::doLeave(std::istringstream& iss, int index)
{
	if (!this->users[index - 1].isLoged())
		throw std::runtime_error("Error: you are not authenticated!\nUse PASS, NICK and USER to authenticate!\n");
	std::string channelname = getNextWord(iss);
	User& us = this->users[index - 1];
	Channel& ch = us.findChannel(channelname);

	ch.sendMsg(this->nw, us, "left the channel");
	ch.removeUser(this->nw, us);
	if (ch.getUserCount() == 0)
		this->channels.erase(this->channels.find(channelname));
	this->nw.sendMsg(us.getID(), "You left the channel\n");

	return (true);
}

bool Server::doKick(std::istringstream& iss, int index)
{
	if (!this->users[index - 1].isLoged())
		throw std::runtime_error("Error: you are not authenticated!\nUse PASS, NICK and USER to authenticate!\n");
	std::string channelname = getNextWord(iss);
	std::string username = getNextWord(iss);
	Channel& ch = this->users[index - 1].findChannel(channelname);
	if (!ch.isAdmin(this->users[index - 1]))
		throw std::runtime_error("Error: You are not Admin!\n");

	if (this->users[index - 1].getUsername() == username)
		throw std::runtime_error("Error: You can't kick yourself!\n");
	User& us = ch.findUser(username, this->nw);
	ch.removeUser(this->nw, us);
	this->nw.sendMsg(us.getID(), "You have been kicked from " + ch.getChannelName() + " by " + this->users[index - 1].getUsername() + "!\n");
	this->nw.sendMsg(this->users[index - 1].getID(), "You have kicked " + us.getUsername() + "!\n");
	ch.sendMsg(this->nw, this->users[index - 1], "kicked " + us.getUsername());
	return (true);
}

bool Server::doInvite(std::istringstream& iss, int index)
{
	if (!this->users[index - 1].isLoged())
		throw std::runtime_error("Error: you are not authenticated!\nUse PASS, NICK and USER to authenticate!\n");
	std::string username = getNextWord(iss);
	std::string channelname = getNextWord(iss);
	Channel& ch = this->users[index - 1].findChannel(channelname);

	for (size_t i = 0; i < this->users.size(); i++)
	{
		if (this->users[i].getUsername() == username)
		{
			if (i == (size_t)index - 1)
				throw std::runtime_error("Error: You can not invite youself!\n");
			if (ch.alreadyJoined(this->users[i]))
				throw std::runtime_error("Error: This users is alread in the Channel!\n");
			ch.invite(this->users[i]);
			ch.sendMsg(this->nw, this->users[index - 1], "invited " + this->users[i].getUsername());
			this->nw.sendMsg(this->users[index - 1].getID(), "You have invited " + username + "\n");
			this->nw.sendMsg(this->users[i].getID(), "You have been invited to join " + ch.getChannelName() + "\n");
		}
	}
	return (true);
}

bool Server::doTopic(std::istringstream& iss, int index)
{
	if (!this->users[index - 1].isLoged())
		throw std::runtime_error("Error: you are not authenticated!\nUse PASS, NICK and USER to authenticate!\n");
	std::string channelname = getNextWord(iss);
	Channel& ch = this->users[index - 1].findChannel(channelname);
	
	char c = iss.get();
	if (c != '\n' && c != '\r')
	{
		if (!findColon(iss))
			throw std::runtime_error("Error: You have to Specify a start with ':' !\n");
			
		if (ch.getPrivateTopic())
			if (!ch.isAdmin(this->users[index - 1]))
				throw std::runtime_error("Error: You are not allowed to modify the Topic!\n");

		std::string newTopic;
		std::getline(iss, newTopic);
		if (newTopic[newTopic.size() - 1] == '\r')
			newTopic.erase(newTopic.size() - 1);
		ch.setTopic(newTopic);
		this->nw.sendMsg(this->users[index - 1].getID(), "You set the topic to: " + ch.getTopic() + "\n");
		ch.sendMsg(this->nw, this->users[index - 1], "changed the topic to: " + ch.getTopic());
	}
	else
	{
		this->nw.sendMsg(this->users[index - 1].getID(), "The current topic is: " + ch.getTopic() + "\n");
	}
	return (true);
}

bool Server::doMode(std::istringstream& iss, int index)
{
	if (!this->users[index - 1].isLoged())
		throw std::runtime_error("Error: you are not authenticated!\nUse PASS, NICK and USER to authenticate!\n");
	std::string channelname = getNextWord(iss);
	std::string option = getNextWord(iss);
	Channel& ch = this->users[index - 1].findChannel(channelname);
	if (!ch.isAdmin(this->users[index - 1]))
		throw std::runtime_error("Error: You are not Admin!\n");

	if (option == "i" || option == "-i")
	{
		ch.toggelInviteOnly();
		if (ch.getInviteOnly())
			nw.sendMsg(this->users[index - 1].getID(), "You set the channel to invite only\n");
		else
			nw.sendMsg(this->users[index - 1].getID(), "You removed invite only from this channel\n");
	}
	else if (option == "t" || option == "-t")
	{
		ch.toggelPrivateTopic();
		if (ch.getPrivateTopic())
		{
			nw.sendMsg(this->users[index - 1].getID(), "You set the channel topic private\n");
			ch.sendMsg(this->nw, this->users[index - 1], "set the channel topic private");
		}
		else
		{
			nw.sendMsg(this->users[index - 1].getID(), "You set the channel topic public\n");
			ch.sendMsg(this->nw, this->users[index - 1], "set the channel topic public");
		}
	}
	else if (option == "k" || option == "-k")
	{
		std::string password = getNextWord(iss);

		if (password.size() == 0)
		{
			ch.setPassword(password);
			nw.sendMsg(this->users[index - 1].getID(), "You have removed the channel Password\n");
		}
		else
		{
			ch.setPassword(password);
			nw.sendMsg(this->users[index - 1].getID(), "You have set the channel Password to '" + password + "'\n");
		}
	}
	else if (option == "o" || option == "-o")
	{
		std::string name = getNextWord(iss);
		User& us = ch.findUser(name, this->nw);

		if (this->users[index - 1].getID() == us.getID())
			throw std::runtime_error("Error: You can not modify your own account!\n");

		if (ch.isAdmin(us))
		{
			ch.removeAdmin(this->nw, us);
			this->nw.sendMsg(us.getID(), "You are no longer a Admin\n");
			this->nw.sendMsg(this->users[index - 1].getID(), us.getUsername() + " is no longer a Admin\n");
		}
		else
		{
			ch.addAnotherAdmin(us);
			this->nw.sendMsg(us.getID(), "You are a Administrator now\n");
			this->nw.sendMsg(this->users[index - 1].getID(), "You promoted " + us.getUsername() + " to a Admin\n");
		}
		
	}
	else if (option == "l" || option == "-l")
	{
		std::string num = getNextWord(iss);

		if (num.size() != 0)
		{
			unsigned int number;

			if (!Parse::isNumeric(num))
				throw std::runtime_error("Error: expected a positive integer\n");
			std::stringstream ss(num);
			if (!(ss >> number))
				throw std::runtime_error("Error: Overflow\n");
			ch.setMaxUsers(number);
			if (number == 0)
				this->nw.sendMsg(this->users[index - 1].getID(), "The Channel user limit is removed!\n");
			else
				this->nw.sendMsg(this->users[index - 1].getID(), "The Channel limit is set to " + num + "\n");
		}
		else
		{
			ch.setMaxUsers(0);
			this->nw.sendMsg(this->users[index - 1].getID(), "The Channel user limit is removed!\n");
		}
	}
	else
		throw std::runtime_error("Error: invalide option!\n");
	return (true);
}
