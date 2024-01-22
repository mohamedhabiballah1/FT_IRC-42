#include "../include/Server.hpp"

std::string Server::NumericReply(std::string nick, std::string num, std::string msg)
{
    std::string prefix = ":youlhabib@127.0.0.1";
    if (nick.empty())
        nick = "guest";
    return (prefix + " " + num + " " + nick + " " + msg + "\r\n");
}

std::string generatReply(std::string color, std::string nick, std::string err)
{
    if (nick.empty())
        nick = "*";
    return (color + nick + " " + err + RESET +"\n");
}

std::string Server::generatRepl(int i, std::string num, std::string msg)
{
    std::string prefix = this->client[i].getPrefix();
    std::string nick = this->client[i].getNickName();
    if (nick.empty())
        nick = "guest";
    if (prefix.empty())
        prefix = ":youlhafi@habib";
    return (prefix + " " + num + " " + nick + " " + msg + "\r\n");
}

bool    Server::checkNickName(std::string nick)
{
    if (!isalpha(nick[0]))
        return false;
    for (size_t i = 0 ; i < nick.length() ; i++){
        if (!isalnum(nick[i]) && nick[i] != '-' && nick[i] != '_' && nick[i] != '\\')
            return false;
    }
    return true;
}

bool    Server::checkChannelName(std::string channelName)
{
    if (channelName[0] != '#' && channelName[0] != '&') {
        return false;
    }
    if (channelName.length() == 1)
        return false;
    for (size_t i = 1; i < channelName.length(); ++i) {
        char ch = channelName[i];
        if (!std::isalnum(ch) && ch != '-' && ch != '_' 
            && ch != '+' && ch != '!' && ch != '.') {
            return false;
        }
    }

    return true;
}

bool    Server::nickAlreadyUsed(std::string nick)
{
    std::map<int , Client >::iterator it;
    for (it = this->client.begin(); it != this->client.end(); ++it){
        if (it->second.getNickName() == nick)
            return false ;
    }
    return true ;
}

int Server::getFdOfTarget(std::string nick)
{
    std::map<int , Client >::iterator it;

    for (it = this->client.begin(); it != this->client.end(); ++it){
        if (it->second.getNickName() == nick)
            return it->second.getFdClient() ;
    }
    return -1;
}

std::vector<std::string> Server::splitChannelKeys(std::string splited)
{
    std::vector<std::string> ret;
    std::stringstream ss(splited);
    std::string help;
 
    while (std::getline(ss, help, ',')){
        ret.push_back(help);
    }
    return ret;
}

std::string Server::getNames(int i, std::string name)
{
    std::map<int, Client >cli = this->channels[name].getMembers();
    std::map<int, Client >oper = this->channels[name].getOpers();
    std::string symbol = "=";
    std::string rep;

    if (this->channels[name].getModeInvite())
        symbol = "@";
    rep = ":youlhabib@127.0.0.1 353 " + this->client[i].getNickName() + " "
    + symbol + " " + name + " :";

    for (std::map<int, Client>::iterator it = oper.begin() ; it != oper.end() ; it++){
        rep.append("@");
        rep.append(it->second.getNickName());
        rep.append(" ");
    }
    for (std::map<int, Client>::iterator it = cli.begin() ; it != cli.end() ; it++){
        std::string mem = it->second.getNickName();
        int index = 0;
        for (std::map<int, Client>::iterator ip = oper.begin() ; ip != oper.end() ; ip++){
            if (ip->second.getNickName() == mem){
                index = 1;
            }
        }
        if (index == 0){
            rep.append(it->second.getNickName());
            rep.append(" ");
        }
    }
    rep.append("\n");
    return rep;
}