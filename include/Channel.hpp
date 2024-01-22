#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include "Client.hpp"

class Client;

class Channel
{
    private:
        std::string channelName;
        std::string channelKey;
        std::string Topic;
        std::string TopicSeter;
        std::time_t TopicSetat;
        
        std::map<int , Client > members;
        std::map<int , Client > operators;
        std::string limitMembers;
        std::vector<std::string> listInvite;
        int nbMembers;

        bool modeInvite;
        bool modeTopic;
        bool modeKey;
        bool modeOper;
        bool modeLimit;
    public:
        Channel();
        Channel(std::string name);
        ~Channel();

        void    addNbMembers();
        void    decNbMembers();
        int     getNbMembers();

        void    setKey(std::string key);
        std::string getKey();

        void    setLimit(std::string lim);
        std::string getLimit();

        void    setTopic(std::string topic);
        std::string getTopic();

        void setTopicSeter(std::string nick);
        std::string getTopicSeter();

        void    setTopicSetat(std::time_t state);
        std::time_t getTopicSetat();

        void    addListInvite(std::string nick);
        void    removeListInvite(std::string nick);
        bool    checkIfInveted(std::string nick);

        void    setChannelName(std::string name);
        std::string getChannelName();
        std::map<int, Client > getMembers();
        std::map<int, Client > getOpers();

        void    addClient(int i, Client client);
        void    addOper(int i, Client client);
        void    removClient(int i);
        void    removOper(int i);

        void sendTopicToAllMembers(int i);
        std::string getModes();

        void    setMode(char c);
        void    removeMode(char c);

        bool getModeInvite();
        bool getModeTopic();
        bool getModeKey();
        bool getModeOper();
        bool getModeLimit();

        void setModeInvite(bool inv);
        void setModeTopic(bool top);
        void setModeKey(bool key);
        void setModeOper(bool oper);
        void setModeLimit(bool lim);

};


#endif