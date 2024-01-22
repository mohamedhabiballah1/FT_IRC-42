#include "../include/Server.hpp"

std::string genratIrcMessage()
{
    std::string message;

    message.append("  ___ ___  ___   ___ ___ _____   _____ ___  \n");
    message.append(" |_ _| _ \\/ __| / __| __| _ \\ \\ / / __| _ \\ \n");
    message.append("  | ||   / (__  \\__ \\ _||   /\\ V /| _||   / \n");
    message.append(" |___|_|_\\\\___| |___/___|_|_\\ \\_/ |___|_|_\\ \n");
    message.append("                                            \n");
    return (message);
}

int parseParameteres(char **av) {
    std::string port = av[0];
    std::string password = av[1];

    if (port.empty() || password.empty())
        return 1;
    for (size_t i = 0; i < port.length(); i++) {
        if (!std::isdigit(port[i]))
            return 1;
    }
    for (size_t i = 0; i < password.length(); i++) {
        if (std::isspace(password[i]))
            return 1;
    }
    int portNum = std::atoi(av[0]);
    if (portNum == 0 || portNum <= 1023)
        return 1;
    return 0;
}

void printColoredText(const std::string& text, const std::string& colorCode) {
    std::cout << colorCode << text << "\033[0m" << std::endl;
}

int main(int argc, char **argv)
{
    std::string ircserv;

    if (argc != 3 || parseParameteres(&argv[1])){
        std::cout << "USAGE : " << RED << "./irc_serv [port] [password]" << RESET << std::endl;
        return 1;
    }
    ircserv = genratIrcMessage();
    printColoredText(ircserv, "\033[32m");
    Server server(argv[1], argv[2]);
    server.startServer();
}