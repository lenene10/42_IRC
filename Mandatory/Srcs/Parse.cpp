#include "../Incl/Parse.hpp"

bool Parse::isNumeric(const std::string& str) {
    for (size_t i = 0; i < str.size(); i++) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

unsigned short Parse::port(const std::string str)
{
	unsigned short port;

	if (!isNumeric(str) && str[0] != '0')
		throw std::runtime_error("Error: Port is non numeric or negative");
	std::stringstream ss(str);
	if (!(ss >> port))
		throw std::runtime_error("Error: Overflow");
	return (port);
}

void Parse::password(const std::string str)
{
	for (size_t i = 0; i < str.size(); i++) {
        if (!std::isprint(str[i])) {
			throw std::runtime_error("Error: String contains a non printable Character");
        }
    }
}