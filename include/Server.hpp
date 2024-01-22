#ifndef SERVER_HPP
#define SERVER_HPP


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <map>
#include <ctime>
#include <fcntl.h>
#include <arpa/inet.h>

#include "Client.hpp"
#include "ParseMessage.hpp"
#include "Client.hpp"
#include "Channel.hpp"


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BUFFER_SIZE 512


class ParseMessage;
class Help;
class Client;
class Channel;

class Server
{
    private:
        std::string name;
        int fdSock;
        int fdNmbr;
        int clientNBR;
        std::string Port;
        std::string PassWord;
        std::map<int, std::string>  clientMessage;
        std::vector<struct pollfd> pfds;
        int index;
        
        ParseMessage inpute;
        std::map<int , Client> client;
        std::map<std::string , Channel> channels;
        
        void    creatSocket();
        void    addToPfds(int newfd);
        void    parseRecv(std::string msg, int i);
        std::string    checkCommand(std::string cmd, int i);
        bool    checkNickName(std::string nick);
        bool    nickAlreadyUsed(std::string nick);
        int     getFdOfTarget(std::string nick);
        std::vector<std::string> splitChannelKeys(std::string splited);
        void    creatChannel(std::string name, int i, std::string key);
        void    joinChannel(std::string name, int i, std::string key);
        bool    findChannel(std::string name);
        bool    findMemberInChannel(std::string channelName, std::string memberName);
        void    removeFromChannel(std::string channelName, std::string memberName, std::string msg);
        std::string    sendMsgToAllMembers(int i, std::string target);
        std::string checkInputTopic(int i);
        std::string checkInputMode(int i);
        bool        checkIfOper(int i, std::string name);
        std::string getTime();
        bool    checkChannelName(std::string channelName);
        std::string sendToAllMembers(int i, std::string target, std::string msg);
        void    deleteFromPfd(int Fd);
        int     memberChannels(int i);
        void    sendToAllchannels(int i, std::string msg);
        std::string getNames(int i, std::string name);
        std::string NumericReply(std::string nick, std::string num, std::string msg);
        std::string sendMode(int i, std::string target, std::string mosestring, std::string member);
        std::string leaveAllChannel(int i);
        std::string sendOperMode(int i, std::string target, std::string msg);

        //commands
        std::string cmdHelp(int i);
        std::string cmdPass(int i);
        std::string cmdNick(int i);
        std::string cmdUser(int i);
        std::string cmdPrivMsg(int i);
        std::string cmdJoin(int i);
        std::string cmdOper(int i);
        std::string cmdPart(int i);
        std::string cmdNotice(int i);
        std::string cmdQuit(int i);
        std::string cmdNames(int i);

        //operators commands
        std::string cmdKick(int i);
        std::string cmdInvite(int i);
        std::string cmdTopic(int i);
        std::string cmdMode(int i);

        //Bot commands
        std::string cmdBot(int i);
        std::string BotHelp(int i);
        std::string randomJoke(int i);
        std::string clientTime(int i);

        std::string generatRepl(int i, std::string num, std::string msg);
    public:
        Server(std::string Port, std::string PassWord);
        ~Server();
        std::string getHostName();
        void    setHostName(std::string name);
        void    startServer();

};

std::string generatReply(std::string color, std::string nick, std::string err);
#endif
