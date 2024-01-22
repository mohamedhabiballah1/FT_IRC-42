#include "../include/Client.hpp"

Client::Client()
{
    this->Authenticate = false;
}
Client::Client(int fd){
    this->Authenticate = false;
    this->registred = false;
    this->isOper = false;
    this->fdClient = fd;
    this->limitChannel = 3;
}

int Client::getFdClient() {return this->fdClient;};

void Client::setAuth(bool auth) {this->Authenticate = auth;}
bool Client::getAuth() {return this->Authenticate;}

void Client::setRegistred(bool reg) {this->registred= reg;}
bool Client::getRegistred() {return this->registred;}

void    Client::setNickName(std::string nick) {this->NickName = nick;}
std::string Client::getNickName() {return this->NickName;}

void    Client::setUserName(std::string user) {this->UserName = user;}
std::string Client::getUserName() {return this->UserName;}

void    Client::setFullName(std::string full) {this->FullNAme = full;}
std::string Client::getFullName() {return this->FullNAme;}

void    Client::setOper(bool oper) {this->isOper = oper;}
bool    Client::getOper() {return this->isOper;}

void    Client::setPrefix(std::string pre) {this->prefix = pre;}
std::string Client::getPrefix() {return this->prefix;}

void    Client::setTime(std::string time) {this->registredTime = time;}
std::string Client::getTime() {return this->registredTime;}

void    Client::setIp(std::string ip) {this->ip = ip;};
std::string Client::getIp() {return this->ip;};


Client::~Client()
{
    
}