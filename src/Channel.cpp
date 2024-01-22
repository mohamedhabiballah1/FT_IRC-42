#include "../include/Channel.hpp"

Channel::Channel()
{

}

Channel::Channel(std::string name)
{
    this->channelName = name;
    this->modeInvite = false;
    this->modeKey = false;
    this->modeOper = false;
    this->modeTopic = false;
    this->modeLimit = false;
    this->nbMembers = 1;
}

void    Channel::setChannelName(std::string name) { this->channelName = name;}
std::string Channel::getChannelName() {return this->channelName;}
std::map<int, Client > Channel::getMembers() {return this->members;}
std::map<int, Client > Channel::getOpers() {return this->operators;}

void    Channel::setTopic(std::string topic) 
{
    if (topic.empty()){
        this->Topic.clear();
        return ;
    }
    this->Topic = topic;    
}

std::string Channel::getTopic() {return this->Topic;}

void    Channel::setTopicSeter(std::string nick) {this->TopicSeter = nick;}
std::string Channel::getTopicSeter() {return this->TopicSeter;}

void    Channel::setLimit(std::string lim) {this->limitMembers = lim;}
std::string Channel::getLimit() {return this->limitMembers;}

void    Channel::setTopicSetat(std::time_t state) {this->TopicSetat = state;}
std::time_t Channel::getTopicSetat() {return this->TopicSetat;}

void    Channel::setKey(std::string key) {this->channelKey = key;}
std::string Channel::getKey() {return this->channelKey;}



void Channel::addNbMembers() {this->nbMembers++;}
void Channel::decNbMembers()
{
    this->nbMembers--;
    if (this->nbMembers < 0)
        this->nbMembers = 0;
}

int Channel::getNbMembers() {return this->nbMembers;}

bool Channel::getModeInvite() {return this->modeInvite;}
bool Channel::getModeKey() {return this->modeKey;}
bool Channel::getModeLimit() {return this->modeLimit;}
bool Channel::getModeOper() {return this->modeOper;}
bool Channel::getModeTopic() {return this->modeTopic;}

void    Channel::setModeInvite(bool inv) {this->modeInvite = inv;}
void    Channel::setModeKey(bool key) {this->modeKey = key;}
void    Channel::setModeLimit(bool lim) {this->modeLimit = lim;}
void    Channel::setModeOper(bool oper) {this->modeInvite = oper;}
void    Channel::setModeTopic(bool top) {this->modeInvite = top;}

void Channel::addClient(int i, Client client) 
{
    this->members.insert(std::pair<int, Client >(i, client));

    std::string rep;
    rep = members[i].getPrefix() + " " + "JOIN " + this->getChannelName() + "\r\n";
    send (this->members[i].getFdClient(), rep.c_str(), rep.length(), 0);
    rep.clear();
    if (!this->Topic.empty()){
        rep = ":youlhabib@127.0.0.1 TOPIC " +  this->getChannelName() + " :" +
        this->getTopic() + "\r\n";
    }
    send (this->members[i].getFdClient(), rep.c_str(), rep.length(), 0);
    rep.clear();
}

void Channel::addOper(int i, Client client) 
{
    this->operators.insert(std::pair<int, Client >(i, client));
}

void Channel::removOper(int i) {this->operators.erase(i);}
void Channel::removClient(int i) {this->members.erase(i);}

void Channel::sendTopicToAllMembers(int i)
{
    std::map<int, Client >::iterator it = this->members.begin();

    while (it != this->members.end()){
        int fd = it->second.getFdClient();
        std::string rep;
        if (this->Topic.empty()){
            rep = ":youlhabib@127.0.0.1 331 " + members[i].getNickName() + " " 
                + this->getChannelName() + " : No topic is set\r\n";
        }
        else{
            rep = ":youlhabib@127.0.0.1 332 " + members[i].getNickName() + " " 
                + this->getChannelName() + " :" + this->getTopic() + "\r\n";
        }
        send (fd, rep.c_str(), rep.length(), 0);
        it++;
    }
}

void    Channel::setMode(char c)
{
    if (c == 'i')
        this->modeInvite = true;
    else if (c == 't')
        this->modeTopic = true;
    else if (c == 'k')
        this->modeKey = true;
    else if (c == 'o'){
        this->modeOper = true;
    }
    else if (c == 'l')
        this->modeLimit = true;
}

void    Channel::removeMode(char c)
{
    if (c == 'i')
        this->modeInvite = false;
    else if (c == 't')
        this->modeTopic = false;
    else if (c == 'k')
        this->modeKey = false;
    else if (c == 'o')
        this->modeOper = false;
    else if (c == 'l')
        this->modeLimit = false;
}

std::string Channel::getModes()
{
    std::string mode = "+";
    if (this->modeInvite)
        mode.append("i");
    if (this->modeTopic)
        mode.append("t");
    if (this->modeKey)
        mode.append("k");
    if (this->modeLimit)
        mode.append("l");
    return mode;
}

void    Channel::addListInvite(std::string nick)
{
    this->listInvite.push_back(nick);
}

void    Channel::removeListInvite(std::string nick)
{
    for (size_t i = 0 ; i < this->listInvite.size() ; i++)
    {
        if (this->listInvite.at(i) == nick)
        {
            this->listInvite.at(i).erase();
            break;
        }
    }
}

bool    Channel::checkIfInveted(std::string nick)
{
    for (size_t i = 0 ; i < this->listInvite.size() ; i++)
    {
        if (this->listInvite.at(i) == nick)
            return true;
    }
    return false;
}

Channel::~Channel()
{

}