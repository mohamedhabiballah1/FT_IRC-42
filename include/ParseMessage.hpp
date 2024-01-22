#ifndef PARSEMESSAGE_HPP
#define PARSEMESSAGE_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <poll.h>
#include <vector>


class ParseMessage
{
    private:
        std::string prefix;
        std::string command;
        std::vector<std::string> parameters;
        std::string trailing;

    public:
        ParseMessage();
        ~ParseMessage();

        void parse(std::string msg);
        void emptyMsg(void);

        std::string getPrefix();
        std::string getCommand();
        std::vector<std::string> getParam();
        std::string getTrailing();
};

#endif