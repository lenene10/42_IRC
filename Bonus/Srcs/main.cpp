#include "../Incl/Parse.hpp"
#include "../Incl/Server.hpp"
#include <iostream>
#include <stdexcept>
#include <csignal>
#include <unistd.h>

void sighandler(int signum)
{
	(void)signum;
	throw std::runtime_error("");
}

int main(int argc, char** argv)
{
	unsigned short port;

	try
	{
		if (argc != 3)
			throw std::runtime_error("Error: Invalide Arguments");
		port = Parse::port(argv[1]);
		Parse::password(argv[2]);
	} 
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	try
	{
		signal(SIGINT, sighandler);

		Server server(port, argv[2]);
		server.run();
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return (0);
}