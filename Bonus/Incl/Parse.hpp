#pragma once

#include <string>
#include <sstream>
#include <stdexcept>

class Parse
{
	public:
		static bool isNumeric(const std::string& str);
		static unsigned short port(const std::string);
		static void password(const std::string str);
};
