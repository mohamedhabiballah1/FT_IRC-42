#include "../include/ParseMessage.hpp"

ParseMessage::ParseMessage()
{

}

std::string ParseMessage::getPrefix(){return this->prefix;}
std::string ParseMessage::getCommand(){return this->command;}
std::string ParseMessage::getTrailing(){return this->trailing;}
std::vector<std::string> ParseMessage::getParam(){return this->parameters;}

void    ParseMessage::parse(std::string message)
{
    /* HABIBALLAH */

    size_t spacePos = message.find(' ');
    if (spacePos == std::string::npos) {
        this->command = message;
        for (size_t i = 0; i < this->command.length(); i++) {
            this->command[i] = std::toupper(this->command[i]);
        }
        return ;
    }

    this->command = message.substr(0, spacePos);
    for (size_t i = 0; i < this->command.length(); i++) {
        this->command[i] = std::toupper(this->command[i]);
    }
    std::string remaining = message.substr(spacePos + 1);
    size_t colonPos = remaining.find(':');
    if (colonPos != std::string::npos) {
        std::istringstream paramStream(remaining.substr(0, colonPos - 1));
        std::string params;
        while (paramStream >> params) {
            this->parameters.push_back(params);
        }
        if (this->command == "TOPIC") {
            this->trailing = remaining.substr(colonPos);
        } else {
            this->trailing = remaining.substr(colonPos + 1);
        }
    } else {
        std::istringstream paramStream(remaining);
        std::string params;
        while (paramStream >> params) {
            this->parameters.push_back(params);
        }
    }
    message.clear();
}

void    ParseMessage::emptyMsg()
{
    this->prefix.clear();
    this->command.clear();
    this->trailing.clear();
    this->parameters.clear();
}

ParseMessage::~ParseMessage()
{

}