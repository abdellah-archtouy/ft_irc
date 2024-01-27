#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <limits>
#include <netdb.h>
#include <strings.h>
#include <poll.h>
#include <map>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include "User.hpp"
#include "Channels.hpp"

// #define TOPIC 0
// #define KICK 1
// #define MODE 2
// #define INVITE 3
// #define JOIN 4


#define RPL_WELCOME(HOST , USER) (HOST  + " 001 " + USER + " :Welcome to the Internet Relay Network user!user@10.11.3.7" + "\r\n")
#define ERR_PASSWDMISMATCH(HOST , USER) (HOST  + " 464 " + USER + " :Password incorrect" + "\r\n")
#define ERR_NEEDMOREPARAMS(HOST , USER) (HOST  + " 461 " + USER + " :Not enough parameters" + "\r\n")
#define ERR_NOTREGISTERED(HOST , USER) (HOST  + " 451 " + USER + " :You have not registered" + "\r\n")
#define ERR_ALREADYREGISTERED(HOST , USER) (HOST  + " 462 " + USER + " :You may not reregister" + "\r\n")
#define ERR_NONICKNAMEGIVEN(HOST , USER) (HOST  + " 431 " + USER + " :No nickname given" + "\r\n")
#define ERR_ERRONEUSNICKNAME(HOST , USER) (HOST  + " 432 " + USER + " :Erroneus nickname" + "\r\n")
#define ERR_NICKNAMEINUSE(HOST , USER) (HOST  + " 433 " + USER + " :Nickname is already in use" + "\r\n")
#define ERR_BADCHANNELKEY(HOST , USER) (HOST  + " 475 " + USER + " :Cannot join channel (+k)" + "\r\n")
#define ERR_NOTEXTTOSEND(HOST , USER) (HOST  + " 412 " + USER + " :No text to send" + "\r\n")
#define ERR_CANNOTSENDTOCHAN(HOST , USER) (HOST  + " 404 " + USER + " :Cannot send to channel" + "\r\n")

#define PRIVMSG(NICK, USER, HOST, COMMAND) (":" + NICK  + "!~" + USER + "@" + HOST + " " + COMMAND + "\r\n")


class Server
{
private:
    std::string _pass;
    std::string _port;
    int serversocket;
    struct pollfd *fd;
    std::vector<pollfd> fds;
    std::map<int, User *> clients;
public:
    std::vector<Channels> Channel;
    Server(/* args */);
    void polling(void);
    std::map<int, User *> get_clients();
    std::vector<pollfd> get_fds();
    void binding(std::string);
    std::string get_host();
    int ft_check_auten(std::map<int, User *> clients, int);
    int ft_get_buffer(std::vector<pollfd> &fd, std::map<int , User *> &clients, int i);
    int kick_out_client(std::vector<pollfd> &fds, std::map<int , User *> &clients, int i, std::vector<pollfd>::iterator it);
    int add_client(std::vector<pollfd> &fds, std::map<int , User *> &clients);
    void Commands(int socket);
    ~Server();
};


#endif