#include "../include/Server.hpp"

std::string Server::cmdBot(int i)
{
    if (this->inpute.getParam().empty())
        return this->BotHelp(i);
    else if (this->inpute.getParam().at(0) == "joke")
        return this->randomJoke(i);
    else if (this->inpute.getParam().at(0) == "time")
        return this->clientTime(i);
    return "";
}

std::string Server::BotHelp(int i)
{
    std::string help;

    help =":bot!joke@localhost NOTICE " + this->client[i].getNickName() + " :**Welcome ["+ this->client[i].getNickName() + "] to Bot**\r\n";
    send(this->client[i].getFdClient(), help.c_str(), help.length(), 0);
    return "";
}

std::string Server::randomJoke(int i)
{
    std::map<int , std::string> jokes;
    std::time_t currentime ;
    std::time(&currentime);

    int rand = currentime % 9;
    jokes.insert(std::pair<int , std::string>(1,"Why don't scientists trust atoms? Because they make up everything!"));
    jokes.insert(std::pair<int , std::string>(2,"I told my computer I needed a break, and now it won't stop sending me vacation ads."));
    jokes.insert(std::pair<int , std::string>(3,"Why did the coffee file a police report? It got mugged!"));
    jokes.insert(std::pair<int , std::string>(4,"Parallel lines have so much in common. It's a shame they'll never meet."));
    jokes.insert(std::pair<int , std::string>(5,"I only know 25 letters of the alphabet. I don't know y."));
    jokes.insert(std::pair<int , std::string>(6,"What do you call fake spaghetti? An impasta."));
    jokes.insert(std::pair<int , std::string>(7,"Why did the scarecrow win an award? Because he was outstanding in his field!"));
    jokes.insert(std::pair<int , std::string>(8,"How does a penguin build its house? Igloos it together!"));
    jokes.insert(std::pair<int , std::string>(9,"I used to play piano by ear, but now I use my hands and fingers."));

    std::string getrand = jokes[rand];
    std::string rep = ":bot!joke@localhost NOTICE " + this->client[i].getNickName() + " :"+ getrand + "\r\n";
    send(this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);
    return "";
}

std::string Server::clientTime(int i)
{
    std::string rep;

    if (this->inpute.getParam().size() < 2){
        rep = ":bot!joke@localhost NOTICE " + this->client[i].getNickName() + " :USAGE BOT time + [Nickname].\r\n";
        send(this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);
        return "";
    }
    else{
        int fd = this->getFdOfTarget(this->inpute.getParam().at(1));
        if (fd == -1){
            rep = ":bot!joke@localhost NOTICE " + this->client[i].getNickName() + " :There was no such nickname\r\n";
            send(this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);   
            return "";
        }
        std::string nick = this->inpute.getParam().at(1);
        rep = ":bot!joke@localhost NOTICE " + this->client[i].getNickName() + " :The registred time of " + nick + " is [" + this->client[fd].getTime() +"]\r\n";
        send(this->client[i].getFdClient(), rep.c_str(), rep.length(), 0);      
        return "";
    }
}