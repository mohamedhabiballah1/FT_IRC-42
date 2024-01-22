#include "../include/Server.hpp"

void    Server::creatChannel(std::string name, int i, std::string key)
{
    std::string rep;

    this->channels.insert(std::pair<std::string, Channel> (name, Channel(name)));
    this->channels[name].addClient(i, this->client[i]);
    this->channels[name].addOper(i, this->client[i]);
    if(!key.empty()){
        this->channels[name].setKey(key);
        this->channels[name].setModeKey(true);
    }
   rep = this->getNames(i, name);
    send(i, rep.c_str(), rep.length(), 0);
    rep = NumericReply(this->client[i].getNickName(), "366",name + " :End of /NAMES list");
    send(i, rep.c_str(), rep.length(), 0);
    std::cout <<  BLUE << "[" << this->getTime() << " " << name << "] " << RESET << "a new channel has been created"<< std::endl;
}

void Server::joinChannel(std::string name, int i, std::string key)
{
    std::string rep;


    if(this->channels[name].getModeKey()){
        if (key.empty() || this->channels[name].getKey() != key){
            std::string rep = NumericReply(this->client[i].getNickName(), "475", name +
            " :Cannot join channel (+k)");
            send(this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);
            return ;
        }
    }
    if (this->channels[name].getModeLimit()){
        if (this->channels[name].getNbMembers() >=  atoi(this->channels[name].getLimit().c_str())){
        std::string rep = NumericReply(this->client[i].getNickName(), "471" ,name + " :Cannot join channel (+l)");
        send(this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);
            return ;
        }

    }
    this->channels[name].addNbMembers();
    this->sendToAllMembers(i, name, " JOIN " + name);
    this->channels[name].addClient(i, this->client[i]);
    rep = this->getNames(i, name);
    send(i, rep.c_str(), rep.length(), 0);
    rep = NumericReply(this->client[i].getNickName(), "366",name + " :End of /NAMES list");
    send(i, rep.c_str(), rep.length(), 0);
}

bool Server::findChannel(std::string name)
{
    std::map<std::string, Channel>::iterator it = this->channels.find(name);
    if (it != this->channels.end())
        return true;
    return false;
}

bool Server::findMemberInChannel(std::string channelName, std::string memberName)
{
    std::map<int, Client > cli;
    cli = this->channels[channelName].getMembers();

    int fd = this->getFdOfTarget(memberName);
    if (fd == -1)
        return false;
    std::map<int , Client >::iterator it = cli.find(fd);
    if (it != cli.end())
        return true;
    return false;
}

void    Server::removeFromChannel(std::string channelName, std::string memberName, std::string msg)
{
    int fd = this->getFdOfTarget(memberName);
    this->sendToAllMembers(fd, channelName, msg);
    this->channels[channelName].removClient(fd);
    this->channels[channelName].decNbMembers();

    std::map<int, Client > oper;
    oper = this->channels[channelName].getOpers();
    std::map<int , Client >::iterator it = oper.find(fd);
    if (it != oper.end()){
        this->channels[channelName].removOper(fd);
    }

    if (this->channels[channelName].getOpers().size() < 1){
        std::map<int, Client > cli;
        cli = this->channels[channelName].getMembers();
        std::map<int , Client >::iterator it = cli.begin();
        if (it != cli.end())
            this->channels[channelName].addOper(it->first, it->second);
    }

    if (this->inpute.getCommand() == "PART"){
        if (this->channels[channelName].getNbMembers() == 0 ){
            std::map<std::string, Channel>::iterator It = this->channels.find(channelName);
            if (It != this->channels.end()) {
                this->channels.erase(It);
            }
        }
    }
}

std::string Server::sendMsgToAllMembers(int i, std::string target)
{
    std::string subtarget;
    std::map<int, Client > client;
    std::map<int, Client >::iterator it;
    int index = 0;

    if (target[0] == '%' && target[1] == '#'){
        subtarget = target.substr(1, target.length() - 1);
        index = 1 ;
    }
    else if (target[0] == '@' && target[1] == '%'){
        subtarget = target.substr(2, target.length() - 2);
        index = 1;
    }
    else
        subtarget = target;
    if (!this->findChannel(subtarget))
        return NumericReply(this->client[i].getNickName(), "401", ":No such nick/channel");
    else if (!this->findMemberInChannel(subtarget, this->client[i].getNickName()))
        return NumericReply(this->client[i].getNickName(), "442", this->inpute.getParam().at(0) + " :You're not on that channel");
    if (index == 0){
        client = this->channels[subtarget].getMembers();
        it = client.begin();
    }else{
        client = this->channels[subtarget].getOpers();
        it = client.begin();
    }

    std::string rep;

    while (it != client.end()){
        rep.append(this->client[i].getPrefix());
        rep.append(" ");
        rep.append(this->inpute.getCommand());
        rep.append(" ");
        rep.append(target);
        rep.append(" :");
        rep.append(this->inpute.getTrailing());
        rep.append("\r\n");
        int fdTarget = it->second.getFdClient();
        if (fdTarget != i)
            send(fdTarget, rep.c_str(), rep.length(), 0);
        it++;
        rep.clear();
    }
    return "";
}

std::string Server::sendToAllMembers(int i, std::string target, std::string msg)
{
    if (!this->findChannel(target))
        return NumericReply(this->client[i].getNickName(), "401", ":No such nick/channel");
    
    std::map<int, Client > client = this->channels[target].getMembers();
    std::map<int, Client >::iterator it = client.begin();

    std::string rep = this->client[i].getPrefix() + msg + "\r\n";
    while (it != client.end()){

        int fdTarget = it->second.getFdClient();
            send(fdTarget, rep.c_str(), rep.length(), 0);
        it++;
    }
    return "";
}

std::string Server::sendMode(int i, std::string target, std::string modestring, std::string member)
{
    if (!this->findChannel(target))
        return NumericReply(this->client[i].getNickName(), "401", ":No such nick/channel");
    
    std::map<int, Client > client = this->channels[target].getMembers();
    std::map<int, Client >::iterator it = client.begin();

    std::string rep = ":youlhabib@127.0.0.1 324 " + this->client[i].getNickName() +" "+ target 
     +" "+ modestring +" "+ member + "\r\n";
    while (it != client.end()){

        int fdTarget = it->second.getFdClient();
            send(fdTarget, rep.c_str(), rep.length(), 0);
        it++;
    }
    return "";
}

std::string Server::sendOperMode(int i, std::string target, std::string msg)
{
    if (!this->findChannel(target))
        return NumericReply(this->client[i].getNickName(), "401", ":No such nick/channel");
    
    std::map<int, Client > client = this->channels[target].getMembers();
    std::map<int, Client >::iterator it = client.begin();

    while (it != client.end()){

        int fdTarget = it->second.getFdClient();
            send(fdTarget, msg.c_str(), msg.length(), 0);
        it++;
    }
    return "";
}

void    Server::sendToAllchannels(int i, std::string msg)
{
    std::map<std::string, Channel>::iterator it = channels.begin();
    std::string rep;

    while (it != channels.end()){
        if (this->findMemberInChannel(it->first, this->client[i].getNickName())){
            this->sendToAllMembers(i, it->first, msg);
        }
        it++;
    }
}

std::string Server::checkInputTopic(int i)
{
    if (this->inpute.getTrailing().empty()){
        if (this->channels[this->inpute.getParam().at(0)].getTopic().empty())
            return NumericReply(this->client[i].getNickName(), "331",
            this->channels[this->inpute.getParam().at(0)].getChannelName() + " :No topic is set");
        else{
            return NumericReply(this->client[i].getNickName(), "332", 
            this->channels[this->inpute.getParam().at(0)].getChannelName() + " :" +
            this->channels[this->inpute.getParam().at(0)].getTopic());
        }

    }

    if (this->channels[this->inpute.getParam().at(0)].getModeTopic())
        if (!this->checkIfOper(i, this->inpute.getParam().at(0)))
            return NumericReply(this->client[i].getNickName(), "482",
                this->channels[this->inpute.getParam().at(0)].getChannelName() + " :You're not channel operator");

    if (this->inpute.getTrailing().length() == 1 && this->inpute.getTrailing()[0] == ':'){
        std::time_t time = std::time(nullptr);
        this->channels[this->inpute.getParam().at(0)].setTopic("");
        this->channels[this->inpute.getParam().at(0)].setTopicSeter(this->client[i].getNickName());
        this->channels[this->inpute.getParam().at(0)].setTopicSetat(time);
        this->channels[this->inpute.getParam().at(0)].sendTopicToAllMembers(i);

    }
    else{
        std::time_t time = std::time(nullptr);

        std::string remaining = this->inpute.getTrailing().substr(1, (this->inpute.getTrailing().length() - 1));
        this->channels[this->inpute.getParam().at(0)].setTopic(remaining);
        this->channels[this->inpute.getParam().at(0)].setTopicSeter(this->client[i].getNickName());
        this->channels[this->inpute.getParam().at(0)].setTopicSetat(time);
        this->channels[this->inpute.getParam().at(0)].sendTopicToAllMembers(i);

    } 
    return "";
}


std::string Server::checkInputMode(int i)
{
    std::string target = this->inpute.getParam().at(0);
    std::string modeString = this->inpute.getParam().at(1);

    std::string plus;
    std::string mines;
    std::string limit;

    size_t plusPos = modeString.find("+");
    size_t minesPos = modeString.find("-");
    if (plusPos != std::string::npos)
        plus = modeString.substr(plusPos, modeString.length() - plusPos);
    if (minesPos != std::string::npos)
        mines = modeString.substr(minesPos, plusPos);
   
    if (modeString.length() < 2)
        return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
    if (plus.find("kl") != std::string::npos){
        if (this->inpute.getParam().size() < 4)
            return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
    }

    for(size_t j = 1; j < plus.size(); j++){
        if (plus[j] == 'o'){
            if (this->inpute.getParam().size() < 3)
                return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
            std::string newOper = this->inpute.getParam().at(2);
            int         fdnewOPer = this->getFdOfTarget(newOper);
            if (fdnewOPer == -1){
                return  NumericReply(this->client[i].getNickName(), "401", newOper + " :No such nick/channel");
            }
            this->channels[target].addOper(fdnewOPer, this->client[fdnewOPer]);
            std::string rep = ":youlhabib@127.0.0.1 MODE " + target + " +o " + newOper + "\r\n";
            sendOperMode(i, target, rep);
            return "";
        }
        else if (plus[j] == 'k'){
            std::cout << "good" <<std::endl;
            if (this->inpute.getParam().size() < 3)
                return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
            std::string newKey = this->inpute.getParam().at(2);
            if (newKey.find(' ') != std::string::npos)
                return NumericReply(this->client[i].getNickName(), "525", ":Key is not well-formed");
            this->channels[target].setKey(newKey);
        }
        else if(plus[j] == 'l'){
            if (this->inpute.getParam().size() < 3)
                return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
            if (plus.find("k") != std::string::npos){
                    limit = this->inpute.getParam().at(3);
            }
            else{
                limit = this->inpute.getParam().at(2);
            }
            this->channels[target].setLimit(limit);
        }
        this->channels[target].setMode(plus[j]);
    }
    for(size_t j = 1; j < mines.size(); j++){
        this->channels[target].removeMode(mines[j]);
        if (mines.find('o') != std::string::npos){
            std::string newOper = this->inpute.getParam().at(2);
            int         fdnewOPer = this->getFdOfTarget(newOper);
            if (fdnewOPer == -1){
                return  NumericReply(this->client[i].getNickName(), "401", newOper + " :No such nick/channel");
            }
            if (fdnewOPer != i){
                this->channels[target].removOper(fdnewOPer);
                std::string rep = ":youlhabib@127.0.0.1 MODE " + target + " -o " + newOper + "\r\n";
                sendOperMode(i, target, rep);
            }
            return "";
        }
    }
    if (plus.find('o') == std::string::npos || mines.find('o') == std::string::npos)
        sendMode(i, target, this->channels[target].getModes(), "");
    return "";
}

bool    Server::checkIfOper(int i, std::string name)
{
    std::map<int, Client > oper;
    oper = this->channels[name].getOpers();

    int fd = this->getFdOfTarget(this->client[i].getNickName());
    if (fd == -1)
        return false;
    std::map<int , Client >::iterator it = oper.find(fd);
    if (it != oper.end())
        return true;
    return false;
}

std::string Server::getTime()
{
    time_t ctime;
    struct tm *info;
    char    buff[100];

    time(&ctime);
    info = localtime(&ctime);

    strftime(buff, sizeof(buff), "%d-%m-%y %H:%M:%S",info);
    std::string tm(buff);
    return tm;
}

std::string Server::leaveAllChannel(int i)
{
    std::map<std::string, Channel>::iterator it = this->channels.begin();
    std::string rep;

    while (it != channels.end()){
        if (this->findMemberInChannel(it->first, this->client[i].getNickName())){
            this->removeFromChannel(it->first, this->client[i].getNickName()
            , " PART " + it->second.getChannelName());
        } 
        it++;
    }

    it = this->channels.begin();
    while (it != channels.end()){
        if (it->second.getNbMembers() == 0){
            it = this->channels.erase(it);
        }
        else
            it++;
    }
    return "";
}