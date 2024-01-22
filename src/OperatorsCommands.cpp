#include "../include/Server.hpp"

std::string Server::cmdKick(int i)
{
    if (this->client[i].getRegistred()){
        if (this->inpute.getParam().size() < 2)
            return NumericReply(this->client[i].getNickName(), "461",this->inpute.getCommand() + " :Not enough parameters");
        else if(!this->findChannel(this->inpute.getParam().at(0)))
            return NumericReply(this->client[i].getNickName(), "403",this->inpute.getParam().at(0) + " :No such channel");
        else if (!this->findMemberInChannel(this->inpute.getParam().at(0), this->client[i].getNickName()))
            return NumericReply(this->client[i].getNickName(), "442",this->inpute.getParam().at(0) + " :You're not on that channel");
        else if (!this->checkIfOper(i, this->inpute.getParam().at(0)))
                return NumericReply(this->client[i].getNickName(), "482",this->inpute.getParam().at(0) + " :You're not channel operator");
        else if(!this->findMemberInChannel(this->inpute.getParam().at(0),this->inpute.getParam().at(1)))
            return NumericReply(this->client[i].getNickName(), "441",this->inpute.getParam().at(1) + " " +this->inpute.getParam().at(0) + " :They aren't on that channel");
        if (getFdOfTarget(this->inpute.getParam().at(1)) == i)
            return "";
        removeFromChannel(this->inpute.getParam().at(0),this->inpute.getParam().at(1), " KICK " + this->inpute.getParam().at(0)
        + " " + this->inpute.getParam().at(1));

    }
    else
        return NumericReply(this->client[i].getNickName(), "451", ":You need to be registred first");
    return"";
}

std::string Server::cmdInvite(int i)
{
    if (this->client[i].getRegistred()){
        if (this->inpute.getParam().size() < 2)
            return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
        else if(!this->findChannel(this->inpute.getParam().at(1)))
            return NumericReply(this->client[i].getNickName(), "403", ":No such channel");
        else if (!this->findMemberInChannel(this->inpute.getParam().at(1), this->client[i].getNickName()))
            return NumericReply(this->client[i].getNickName(), "442", ":You're not on that channel");
        else if(this->findMemberInChannel(this->inpute.getParam().at(1),this->inpute.getParam().at(0)))
            return NumericReply(this->client[i].getNickName(), "443", this->inpute.getParam().at(0) + " " +
            this->inpute.getParam().at(1) + " :is already on channel");
        else{
                int fd = this->getFdOfTarget(this->inpute.getParam().at(0));
                this->channels[this->inpute.getParam().at(1)].addListInvite(this->inpute.getParam().at(0));
                std::string rep;
                rep = this->client[i].getPrefix() + " " + this->inpute.getCommand() + " " + this->inpute.getParam().at(0) + " " + this->inpute.getParam().at(1) + "\r\n";
                send(fd, rep.c_str(), rep.length(), 0);
                return NumericReply(this->client[i].getNickName(), "341",this->inpute.getParam().at(0) + " " + this->inpute.getParam().at(1));
            }
        
    }
    else
        return NumericReply(this->client[i].getNickName(), "451", ":You need to be registred first");
    return "";
}

std::string Server::cmdTopic(int i)
{
    if (this->client[i].getRegistred()){
        if (this->inpute.getParam().size() < 1)
            return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
        else if(!this->findChannel(this->inpute.getParam().at(0)))
            return NumericReply(this->client[i].getNickName(), "403", ":No such channel");
        else if (!this->findMemberInChannel(this->inpute.getParam().at(0), this->client[i].getNickName()))
            return NumericReply(this->client[i].getNickName(), "442", ":You're not on that channel");
        else
            return this->checkInputTopic(i);      
    }
    else
        return NumericReply(this->client[i].getNickName(), "451", ":You need to be registred first");
    return "";
}

std::string Server::cmdMode(int i)
{
    (void)i;
    if (this->client[i].getRegistred()){
        if (this->inpute.getParam().empty())
            return NumericReply(this->client[i].getNickName(), "461", ":Not enough parameters");
        else if (!this->findChannel(this->inpute.getParam().at(0)))
            return NumericReply(this->client[i].getNickName(), "401", ":No such nick/channel");
        else if (this->inpute.getParam().size() == 1){
            return (NumericReply(this->client[i].getNickName(), "324", this->inpute.getParam().at(0) + " " + this->channels[this->inpute.getParam().at(0)].getModes()));
        }
        else if(!this->checkIfOper(i, this->inpute.getParam().at(0)))
            return NumericReply(this->client[i].getNickName(), "482", this->inpute.getParam().at(0) + " :You're not channel operator");
        else{
            return this->checkInputMode(i);
        }
    }
    else
        return NumericReply(this->client[i].getNickName(), "451", ":You need to be registred first");
    return "";
}