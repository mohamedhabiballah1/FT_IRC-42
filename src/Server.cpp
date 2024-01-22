#include "../include/Server.hpp"

bool shutDown;

Server::Server(std::string Port, std::string PassWord)
{
    shutDown = false;
    this->name = "youl!habib@HolyMoly";
    this->Port = Port;

    this->PassWord = PassWord;
    creatSocket();
    if (fcntl(this->fdSock, F_SETFL, O_NONBLOCK) < 0)
    {
        perror("FCNTL ERROR");
        exit(EXIT_FAILURE);
    }
    struct pollfd sockfd;
    sockfd.fd = this->fdSock;
    sockfd.events = POLLIN;
    this->pfds.push_back(sockfd);
    this->fdNmbr = 1;
    std::cout << YELLOW << "Server is setuped succefully" << RESET << std::endl;
    std::cout << YELLOW << "Server start listening in port " << Port << RESET << std::endl
              << std::endl;
}

void Server::creatSocket()
{
    int status;
    int yes = 1;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, this->Port.c_str(), &hints, &res)) != 0)
    {
        exit(EXIT_FAILURE);
    }

    this->fdSock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (this->fdSock < 0)
    {
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    if (setsockopt(this->fdSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    {
        close(this->fdSock);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    if (bind(this->fdSock, res->ai_addr, res->ai_addrlen) == -1)
    {
        close(this->fdSock);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    if (listen(this->fdSock, 10) == -1)
    {
        close(this->fdSock);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res);
}

void Server::addToPfds(int newfd)
{
    struct pollfd fdNew;
    fdNew.fd = newfd;
    fdNew.events = POLLIN;
    this->pfds.push_back(fdNew);
    this->fdNmbr++;
    this->clientNBR++;
    this->client.insert(std::pair<int, Client >(newfd, Client(newfd)));
}

void shutDownserver()
{
    std::cout << std::endl;
    std::cout << RED << "Server shut down" << RESET << std::endl;
    exit(EXIT_SUCCESS);
}

void Server::deleteFromPfd(int Fd)
{
    std::string nickName = this->client[Fd].getNickName();
    std::map<int, Client >::iterator clientIt = client.find(Fd);
    this->clientMessage.erase(Fd);
    this->leaveAllChannel(Fd);
    this->index = 1;
    if (clientIt != client.end())
    {
        client.erase(clientIt);
    }
    if (nickName.empty())
    {
        std::cout << BLUE << "[" << this->getTime() << " Client " << Fd << "] " << RESET << "Hung Up" << std::endl;
    }
    else
    {
        std::cout << BLUE << "[" << this->getTime() << " " << nickName << "] " << RESET << "Hung Up" << std::endl;
    }
    for (std::vector<struct pollfd>::iterator it = this->pfds.begin(); it != this->pfds.end(); it++)
    {
        if (it->fd == Fd)
        {
            pfds.erase(it);
            break;
        }
    }
    this->fdNmbr--;
    close(Fd);
}

std::string Server::checkCommand(std::string cmd, int j)
{
    std::string cmds[] = {"PASS", "NICK", "USER", "PRIVMSG", "JOIN", "OPER", "KICK",
     "INVITE", "TOPIC", "MODE", "BOT", "PART", "NOTICE", "QUIT", "NAMES"};

    std::string (Server::*ptrFunctions[])(int i) = {&Server::cmdPass, &Server::cmdNick, &Server::cmdUser,
     &Server::cmdPrivMsg, &Server::cmdJoin, &Server::cmdOper, &Server::cmdKick, &Server::cmdInvite, &Server::cmdTopic,
      &Server::cmdMode, &Server::cmdBot, &Server::cmdPart, &Server::cmdNotice, &Server::cmdQuit, &Server::cmdNames};

    for (size_t i = 0; i < 15; i++)
    {
        if (cmd == cmds[i])
            return (this->*ptrFunctions[i])(j);
    }
    return "";
}

void Server::parseRecv(std::string msg, int i)
{
    int fd = this->pfds.at(i).fd;
    std::string ret;

    this->inpute.parse(msg);
    ret = this->checkCommand(this->inpute.getCommand(), fd);
    if (!ret.empty())
        send(fd, ret.c_str(), ret.length(), 0);
    this->inpute.emptyMsg();
}

void signalHandler(int i)
{
    (void)i;
    shutDown = true;
}

std::vector<std::string> splitNewLines(std::string &target)
{
    std::vector<std::string> result;
    std::istringstream targets(target);
    std::string line;

    while (std::getline(targets, line))
    {
        result.push_back(line);
    }
    return result;
}

void Server::startServer()
{

    struct sockaddr_storage their_addr;
    int newfd;
    char buff[512];
    socklen_t addrlen;
    std::string msg;

    signal(SIGTSTP, SIG_IGN);
    signal(SIGINT, signalHandler);
    signal(SIGPIPE, SIG_IGN);

    while (!shutDown)
    {
        int poll_count = poll(&this->pfds.at(0), this->fdNmbr, -1);
        if (poll_count == -1){ continue; }
        for (int i = 0; i < this->fdNmbr; i++)
        {
            this->index = 0;
            if (this->pfds.at(i).revents & POLLIN && !shutDown)
            {
                if (pfds[i].fd == this->fdSock)
                {
                    addrlen = sizeof(their_addr);
                    newfd = accept(this->fdSock, (struct sockaddr *)&their_addr, &addrlen);
                    if (newfd == -1)
                    {
                        perror("ACCEPT FAILS");
                    }
                    else
                    {
                        fcntl(newfd, F_SETFL, O_NONBLOCK);
                        addToPfds(newfd);
                        clientMessage.insert(std::pair<int, std::string>(newfd, ""));
                        struct sockaddr_in *ip = (struct sockaddr_in *)&their_addr;
                        char clientIP[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &(ip->sin_addr), clientIP, INET_ADDRSTRLEN);
                        this->client[newfd].setIp(clientIP);
                        std::cout << BLUE << "[" << this->getTime() << " Client " << newfd << "] " << RESET << "is connected to the server" << std::endl;
                        break;
                    }
                }
                else
                {
                    int bytes = recv(this->pfds.at(i).fd, buff, sizeof(buff), 0);

                    if (bytes >= 0)
                    {
                        if (bytes == 0)
                        {
                            deleteFromPfd(this->pfds.at(i).fd);
                        }
                        else if (bytes > BUFFER_SIZE)
                        {
                            if (send(this->pfds.at(i).fd, "Input too long\n", 16, 0))
                            {
                                perror("SEND ERROR");
                            }
                        }
                        else
                        {
                            msg.append(buff, bytes);
                            this->clientMessage[this->pfds.at(i).fd].append(msg);
                            size_t pos = this->clientMessage[this->pfds.at(i).fd].find("\n");
                            if (pos != std::string::npos)
                            {
                                if (std::count(this->clientMessage[this->pfds.at(i).fd].begin(), this->clientMessage[this->pfds.at(i).fd].end(), '\n') != 1)
                                {
                                    std::vector<std::string> tmp = splitNewLines(this->clientMessage[this->pfds.at(i).fd]);
                                    for (size_t y = 0; y < tmp.size(); y++)
                                    {
                                        this->parseRecv(tmp.at(y), i);
                                    }
                                }
                                else
                                    this->parseRecv(this->clientMessage[this->pfds.at(i).fd].substr(0, (this->clientMessage[this->pfds.at(i).fd].length() - 1)), i);

                                if (this->index == 0)
                                    clientMessage[this->pfds.at(i).fd].clear();
                            }
                            msg.clear();
                        }
                    }
                }
            }
        }
    }
    if (shutDown)
    {
        shutDownserver();
    }
}

std::string Server::getHostName(){return this->name;}
void    Server::setHostName(std::string name){this->name = name;}

Server::~Server()
{
}