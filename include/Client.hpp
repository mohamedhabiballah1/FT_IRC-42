#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Client
{
    private:
        int fdClient;
        std::string NickName;
        std::string UserName;
        std::string FullNAme;
        std::string prefix;
        std::string registredTime;
        std::string ip; 
        int         limitChannel;
        bool Authenticate;
        bool registred;
        bool isOper;
    public:
        Client();
        Client(int fd);
        ~Client();

        int getFdClient();
        
        void    setAuth(bool auth);
        bool    getAuth();

        void setRegistred(bool reg);
        bool getRegistred();

        void    setPrefix(std::string pre);
        std::string getPrefix();

        void    setNickName(std::string nick);
        std::string getNickName();

        void    setUserName(std::string user);
        std::string getUserName();

        void    setFullName(std::string full);
        std::string getFullName();

        void    setOper(bool oper);
        bool    getOper();

        void    setTime(std::string time);
        std::string getTime();

        void    setIp(std::string ip);
        std::string    getIp();
};

#endif