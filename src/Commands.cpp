#include "../include/Server.hpp"


std::string    Server::cmdPass(int i)
{
    if (this->inpute.getParam().empty())
        return (NumericReply(this->client[i].getNickName(), "461", this->inpute.getCommand() +  " :Not enough parameters"));
    else if (this->client[i].getAuth())
        return (NumericReply(this->client[i].getNickName(), "462", ":You may not reregister"));
    else if (this->inpute.getParam().at(0) != this->PassWord)
        return (NumericReply(this->client[i].getNickName(), "464", ":Incorrect password"));
    else
        this->client[i].setAuth(true);
    return "";
}

std::string    Server::cmdNick(int i)
{
    std::string reply;
    if (this->client[i].getAuth()){
        if (this->inpute.getParam().empty()) {
            return (NumericReply(this->client[i].getNickName(), "431", ":No nickname given"));
        } else if(!this->checkNickName(this->inpute.getParam().at(0))) {
            return NumericReply(this->client[i].getNickName(),  "432", this->inpute.getParam().at(0) + " :Erroneus nickname");
        } else if (!this->nickAlreadyUsed(this->inpute.getParam().at(0))) {
            return NumericReply(this->client[i].getNickName(), "433", this->inpute.getParam().at(0) + " :Nickname is already in use");
        } else{
            std::string old = this->client[i].getPrefix() + " NICK " + this->inpute.getParam().at(0);
            this->sendToAllchannels(i, old);
            this->client[i].setNickName(this->inpute.getParam().at(0));
            if (!this->client[i].getPrefix().empty())
                this->client[i].setPrefix(":" + this->client[i].getNickName() + "!" + this->client[i].getUserName() + "@localhost");
            if (this->client[i].getAuth() && !this->client[i].getUserName().empty()){
                if (!this->client[i].getRegistred()){
                    this->client[i].setRegistred(true);
                    std::cout <<  BLUE << "[" << this->getTime() << " " << this->client[i].getNickName() << "] " << RESET << "has registred to the server" << std::endl;
                    reply = NumericReply(this->client[i].getNickName(), "001", ":WELCOME TO IRC NETWORK, " + this->client[i].getNickName());
                    send(i, reply.c_str(), reply.length(), 0);
                    reply = NumericReply(this->client[i].getNickName(), "002", ":Your host is " + this->getHostName() + " running version WAHYAL#SSEL");
                    send(this->client[i].getFdClient(), reply.c_str(), reply.length(), 0);
                    reply = NumericReply(this->client[i].getNickName(), "003", ":This server was created 01-01-2024");
                    send(this->client[i].getFdClient(), reply.c_str(), reply.length(), 0);
                    reply = NumericReply(this->client[i].getNickName(), "004", this->getHostName() + " WAHYAL#SSEL ABCDEFGHIJKLM MLKJIHGFEDCBA ZVBAFTSPNSDGG");
                    send(this->client[i].getFdClient(), reply.c_str(), reply.length(), 0);
                    reply = NumericReply(this->client[i].getNickName(), "005", "PREFIX=(ov)@ CHANTYPES=# CHANMODES=itkol :are supported by this server");
                    send(this->client[i].getFdClient(), reply.c_str(), reply.length(), 0);
                    return "";
                }
            }
            return "";
        }
    }
    return NumericReply(this->client[i].getNickName(), "001", ":You need to authenticate first");
}

std::string   Server::cmdUser(int i)
{
    std::string reply;
    if (this->client[i].getAuth()){
        if (this->inpute.getTrailing().empty()) {
            return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters"); 
        } else if (this->inpute.getParam().size() != 3) {
            return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
        } else if (this->client[i].getRegistred()) {
            return NumericReply(this->client[i].getNickName(), "462", ":You may not reregister");
        } else {
            if (this->inpute.getParam().at(0).length() > 12)
                this->client[i].setUserName(this->inpute.getParam().at(0).substr(0,12));
            else
                this->client[i].setUserName(this->inpute.getParam().at(0));
            this->client[i].setFullName(this->inpute.getTrailing());
            this->client[i].setPrefix(":" + this->client[i].getNickName() + "!" + this->client[i].getUserName() + "@" + this->client[i].getIp());
            
            this->client[i].setTime(this->getTime());
            
            if (this->client[i].getAuth() && !this->client[i].getNickName().empty()){
                this->client[i].setRegistred(true);
                std::cout <<  BLUE << "[" << this->getTime() << " " << this->client[i].getNickName() << "] " << RESET << "has registred to the server" << std::endl;
                    reply = NumericReply(this->client[i].getNickName(), "001", ":WELCOME TO IRC NETWORK, " + this->client[i].getNickName());
                    send(i, reply.c_str(), reply.length(), 0);
                    reply = NumericReply(this->client[i].getNickName(), "002", ":Your host is " + this->getHostName() + " running version WAHYAL#SSEL");
                    send(this->client[i].getFdClient(), reply.c_str(), reply.length(), 0);
                    reply = NumericReply(this->client[i].getNickName(), "003", ":This server was created 01-01-2024");
                    send(this->client[i].getFdClient(), reply.c_str(), reply.length(), 0);
                    reply = NumericReply(this->client[i].getNickName(), "004", this->getHostName() + " WAHYAL#SSEL ABCDEFGHIJKLM MLKJIHGFEDCBA ZVBAFTSPNSDGG");
                    send(this->client[i].getFdClient(), reply.c_str(), reply.length(), 0);
                    reply = NumericReply(this->client[i].getNickName(), "005", "PREFIX=(ov)@ CHANTYPES=# CHANMODES=itkol :are supported by this server");
                    send(this->client[i].getFdClient(), reply.c_str(), reply.length(), 0);
                    return "";
            }
        }
    } else {
        return NumericReply(this->client[i].getNickName(), "001", ":You need to authenticate first");
    }
    return "";
}

std::string Server::cmdPrivMsg(int i)
{
    std::vector<std::string> target;
    int         fdTarget;
    std::string rep;

    if (this->client[i].getRegistred()){
        if (this->inpute.getParam().empty())
            return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
        else if (this->inpute.getTrailing().empty())
            return NumericReply(this->client[i].getNickName(), "412", ":No text to send");
        target = this->splitChannelKeys(this->inpute.getParam().at(0));
   
        for (size_t j = 0 ; j < target.size(); j++){
            if (target.at(j)[0] == '#' || target.at(j)[0] == '%' || target.at(j)[0] == '@'){
                rep = this->sendMsgToAllMembers(i, target.at(j));
                send(i, rep.c_str(), rep.length(), 0);
            }
            else {
                fdTarget = getFdOfTarget(target.at(j));
                if (fdTarget == -1) {
                    rep = NumericReply(this->client[i].getNickName(), "401", target.at(j) + " :No such nick/channel");
                    send(i, rep.c_str(), rep.length(), 0);
                }
                else{
                    rep = this->client[i].getPrefix();
                    rep.append(" " + this->inpute.getCommand() + " " + target.at(0) + " :" + this->inpute.getTrailing() + "\r\n");
                    send(fdTarget, rep.c_str(), rep.length(), 0);
                }
            }
            rep.clear();
        }
        return "";
    }
    return NumericReply(this->client[i].getNickName(), "451", ":You have not registered");;
}

int Server::memberChannels(int i)
{
    int j = 0;
    std::map<std::string, Channel>::iterator it = channels.begin();

    while (it != channels.end())
    {
        if (findMemberInChannel(it->first, this->client[i].getNickName()))
            j++;
        it++;
    }
    return j;
}

std::string Server::cmdJoin(int i)
{
    int index = 0;
    std::vector<std::string>channel;
    std::vector<std::string>keys;
    std::string key = "";
    std::string rep;

    if (this->client[i].getRegistred()) {
        if (this->inpute.getParam().empty())
            return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
        channel = this->splitChannelKeys(this->inpute.getParam().at(0));
        if (this->inpute.getParam().size() == 2)
            keys = this->splitChannelKeys(this->inpute.getParam().at(1));

        for (size_t j = 0; j < channel.size(); j++) {
            index = 0;
            if (channel.at(j) == "0")
                return this->leaveAllChannel(i);
            if (this->memberChannels(i) >= 5) {
                rep = NumericReply(this->client[i].getNickName(), "476", channel.at(j)+ " :You have joined too many channels");
                send (this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);
            }
            else if (!this->checkChannelName(channel.at(j))) {
                rep = NumericReply(this->client[i].getNickName(), "476", channel.at(j)+ " :Bad Channel Mask");
                send (this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);
            }
            else {
                if (j < keys.size())
                    key = keys.at(j);
                for (std::map<std::string, Channel>::iterator it = this->channels.begin() ; it != this->channels.end(); it++) {
                    if (it->second.getChannelName() == channel.at(j)) {
                        if (!this->findMemberInChannel(it->second.getChannelName(), this->client[i].getNickName())){
                            if (!it->second.getModeInvite()) {
                                this->joinChannel(channel.at(j), i, key);
                            } else {
                                if (this->channels[channel.at(j)].checkIfInveted(this->client[i].getNickName())){
                                    this->joinChannel(channel.at(j), i, key);
                                    if (findMemberInChannel(channel.at(j), this->client[i].getNickName()))
                                        this->channels[channel.at(j)].removeListInvite(this->client[i].getNickName());
                                }
                                else{
                                    rep = NumericReply(this->client[i].getNickName(), "474", it->second.getChannelName() + " :Cannot join channel (+i)");
                                    send (this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);
                                }
                            }
                        }
                        index = 1;
                    }
                }
                if (!index) {
                    this->creatChannel(channel.at(j), i, key);
                }
            }
            key.clear();
            rep.clear();
        }
    }
    else
        return NumericReply(this->client[i].getNickName(), "451", ":You have not registered");
    return "";
}

std::string Server::cmdOper(int i)
{
    if (this->client[i].getRegistred()){
        if (this->inpute.getParam().size() != 2)
            return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
        else if (this->inpute.getParam().at(0) != "ADMIN" || this->inpute.getParam().at(1) != "GoodPass")
            return NumericReply(this->client[i].getNickName(), "464", ":Password incorrect");
        else{
            this->client[i].setOper(true);
            return NumericReply(this->client[i].getNickName(), "381", ":You are now an IRC operator");
        }
    }
    return NumericReply(this->client[i].getNickName(), "451", ":You have not registered");
}

std::string Server::cmdPart(int i)
{
    std::vector<std::string>channel;
    std::string rep;
    (void)i;
    if (this->client[i].getRegistred()){
        if (this->inpute.getParam().empty())
            return NumericReply(this->client[i].getNickName(), "461", " :Not enough parameters");
        channel = this->splitChannelKeys(this->inpute.getParam().at(0));
        for (size_t j = 0; j < channel.size(); j++) {
            if (!this->findChannel(channel.at(j))) {
                rep = NumericReply(this->client[i].getNickName(), "403", channel.at(j) +" :No such channel");
                send(this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);
            } else if (!this->findMemberInChannel(channel.at(j), this->client[i].getNickName())) {
                rep = NumericReply(this->client[i].getNickName(), "442", channel.at(j) + " :You're not on that channel");
                send(this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);
            } else {
                this->removeFromChannel(channel.at(j), this->client[i].getNickName(), " PART " +  channel.at(j));
            }
        }
    }
    else
        return NumericReply(this->client[i].getNickName(), "451", ":You have not registered");
    return "";
}

std::string Server::cmdNotice(int i)
{
    std::vector<std::string> target;
    int         fdTarget;
    std::string rep;

    if (this->client[i].getRegistred()){
        if (this->inpute.getParam().empty())
            return "";
        target = this->splitChannelKeys(this->inpute.getParam().at(0));
        for (size_t j = 0 ; j < target.size(); j++){
            if (target.at(j)[0] == '#' || target.at(j)[0] == '%' || target.at(j)[0] == '@') {
                this->sendMsgToAllMembers(i, target.at(j));
            } else {
                fdTarget = getFdOfTarget(target.at(j));
                if (fdTarget == -1) {
                    return "";
                } else {
                    rep = this->client[i].getPrefix();
                    rep.append(" " + this->inpute.getCommand() + " " + target.at(0) + " :" + this->inpute.getTrailing() + "\r\n");
                    send(fdTarget, rep.c_str(), rep.length(), 0);
                }
            }
            rep.clear();
        }
        return "";
    }
    return NumericReply(this->client[i].getNickName(), "451", ":You have not registered");;
}

std::string Server::cmdQuit(int i)
{
    std::map<std::string, Channel>::iterator it = channels.begin();
    std::string rep;
    while (it != channels.end()){
        if (this->findMemberInChannel(it->first, this->client[i].getNickName())){
            if (this->inpute.getTrailing().empty()) {
                this->removeFromChannel(it->first, this->client[i].getNickName(), " Quit");
            } else {
                this->removeFromChannel(it->first, this->client[i].getNickName(), " Quit :" + this->inpute.getTrailing());
            }
        }
        it++;
    }
    this->deleteFromPfd(i);
    return "";
}

std::string Server::cmdNames(int i)
{
    std::vector<std::string> target;
    std::string rep;

    if (this->client[i].getRegistred()){
        if (this->inpute.getParam().empty())
            return NumericReply(this->client[i].getNickName(), "461", " :Not enough parameters");
        target = this->splitChannelKeys(this->inpute.getParam().at(0));
        for (size_t j = 0 ; j < target.size(); j++){
            if (!this->findChannel(target.at(j))){
                rep = NumericReply(this->client[i].getNickName(), "401", target.at(j) + " :No such nick/channel");
                send(i, rep.c_str(), rep.length(), 0);
            } else {
                rep = this->getNames(i, target.at(j));
                send(i, rep.c_str(), rep.length(), 0);
                rep = NumericReply(this->client[i].getNickName(), "366",name + " :End of /NAMES list");
                send(i, rep.c_str(), rep.length(), 0);
            }
        }
    }
    else
        return NumericReply(this->client[i].getNickName(), "451", ":You have not registered");
    return "";
}