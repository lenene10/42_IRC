#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>

#include "User.hpp"
#include "Channel.hpp"
#include "Network.hpp"

class Bot : public User {
public:
    Bot();
    ~Bot();

    void replaceBadWords(std::string& str);
    bool receiveAndProcessMessages(Network& nw, std::vector<User>& users, int index);
    void help(Network& nw, User& us);
};

