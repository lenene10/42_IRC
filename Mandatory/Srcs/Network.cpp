#include "../Incl/Network.hpp"

Network::Network(unsigned short port, std::vector<User>& users) : users(users)
{
	int sockfd = 0;
	if ((sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1)
		throw std::runtime_error(strerror(errno));

	// make the socket reusable after restart
	int reuse = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
		throw std::runtime_error(strerror(errno));

	this->addr.sin_family = AF_INET;
	this->addr.sin_port = htons(port);
	this->addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw std::runtime_error(strerror(errno));

	if (listen(sockfd, 16) == -1)
		throw std::runtime_error(strerror(errno));

	struct pollfd fds;
	fds.fd = sockfd;
	fds.events = POLLIN;
	fds.revents = 0;
	this->fds.push_back(fds);
	this->connections = 1;
}

Network::~Network()
{
	for (int i = 0; i < this->connections; i++)
	{
		close(this->fds[i].fd);
	}
}

void Network::acceptClients()
{
	static int id = 0;

	if (this->fds[0].revents & POLLIN)
	{
		// accepting new connections
		struct pollfd fds;
		socklen_t socklen = sizeof(this->addr);
		fds.fd = accept(this->fds[0].fd, (struct sockaddr*)&this->addr, &socklen);
		if (fds.fd == -1)
			throw std::runtime_error(strerror(errno));

		// adding the new user to the list
		User user(id++);
		this->users.push_back(user);
		this->cach.push_back("");
		std::cout << "Client " << user.getID() << " connected" << std::endl;

		// setting up the socket for poll
		fds.events = POLLIN;
		fds.revents = 0;
		this->fds.push_back(fds);
		this->connections++;
	}
}

void Network::disconnectClient(int index)
{
	this->users[index - 1].disconnect(*this);
	close(this->fds[index].fd);
	this->fds.erase(this->fds.begin() + index);

	// remove the user from the users vector
	std::cout << "Client " << this->users[index - 1].getID() << " Disconnected" << std::endl;
	this->users.erase(this->users.begin() + index - 1);
	this->cach.erase(this->cach.begin() + index - 1);
	this->connections--;
}

int Network::getIndex(int id)
{
	for (size_t i = 0; i < this->users.size(); i++)
	{
		if (this->users[i].getID() == id)
			return (i + 1);
	}
	return (-1);
}

User& Network::getUser(int id)
{
	int index = this->getIndex(id);
	if (index == -1)
		throw std::runtime_error("Error: Index does not exist");
	return (this->users[index - 1]);
}

User& Network::findUser(std::string& name)
{
	for (size_t i = 0; i < this->users.size(); i++)
	{
		if (this->users[i].getUsername() == name)
			return (this->users[i]);
	}
	throw std::runtime_error("Error: User not found\n");
}

int Network::recvMsg(int index)
{
	char buffer[4096];

	int len = recv(this->fds[index].fd, buffer, 4095, 0);
	if (len == -1)
		throw std::runtime_error(strerror(errno));
	buffer[len] = 0;

	if (len == 0 || ((unsigned char)buffer[0] == 0xFF && (unsigned char)buffer[1] == 0xF4)) // the second part is for handeling ctrl-c from telnet
	{
		this->disconnectClient(index);
		return (2);
	}

	std::string str(this->cach[index - 1] + buffer);
	this->cach[index - 1] = str;
	if (str[str.size() - 1] == '\n')
		return (0);
	return (1);
}

std::string& Network::getCach(int index)
{
	return (this->cach[index - 1]);
}

void Network::clearCach(int index)
{
	this->cach[index - 1] = "";
}

void Network::sendMsg(int id, const std::string& msg)
{
	int index = this->getIndex(id);
	if (index == -1)
		throw std::runtime_error("Error: Index does not exist");

	int len = send(this->fds[index].fd, msg.c_str(), msg.size(), 0);
	if (len == -1)
		throw std::runtime_error(strerror(errno));
}

struct pollfd* Network::getFDs()
{
	return (this->fds.data());
}

int Network::getConnections()
{
	return (this->connections);
}