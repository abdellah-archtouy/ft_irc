#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <iostream>
#include <cstring>
#include <ctime>
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

#define FORMA(USER , NICK , IP) (NICK  + "!~" + USER + "@" + IP)
#define RPL_WELCOME(HOST , USER , IP) (HOST  + " 001 " + USER + " :Welcome to the Internet Relay Network " + IP + "\r\n")
#define RPL_NOTOPIC(HOST , USER, CHANNEL) (HOST  + " 331 " + USER + " " + CHANNEL + " :No topic is set" + "\r\n")
#define RPL_TOPIC(HOST, USER, CHANNEL, TOPIC) (HOST + " 332 " + USER + " " + CHANNEL + " " + TOPIC + "\r\n")
#define RPL_NAMREPLY(USER, CHANNEL, SYMBOLE, HOST, STRING) (HOST + " 353 " + USER + SYMBOLE + CHANNEL + " :" + STRING + "\r\n")
#define RPL_ENDOFNAMES(USER, CHANNEL, HOST) (HOST + " 366 " + USER + " " + CHANNEL + " :End of NAMES list" + "\r\n")
#define RPL_TOPICWHOTIME(USER, CHANNEL, HOST, NICKUSER, TIME) (HOST + " 333 " + USER + " " + CHANNEL + " " + NICKUSER + " " + TIME + "\r\n")
#define RPL_CHANNELMODEIS(USER, CHANNEL, HOST, STRING) (HOST + " 324 " + USER + " " + CHANNEL + " " + STRING + "\r\n")

#define ERR_PASSWDMISMATCH(HOST , USER) (HOST  + " 464 " + USER + " :Password incorrect" + "\r\n")
#define ERR_INVITEONLYCHAN(HOST , USER, CHANNEL) (HOST  + " 473 " + USER + " " + CHANNEL + " :Cannot join channel (+i)" + "\r\n")
#define ERR_NEEDMOREPARAMS(HOST , USER) (HOST  + " 461 " + USER + " :Not enough parameters" + "\r\n")
#define ERR_NOTREGISTERED(HOST , USER) (HOST  + " 451 " + USER + " :You have not registered" + "\r\n")
#define ERR_ALREADYREGISTERED(HOST , USER) (HOST  + " 462 " + USER + " :You may not reregister" + "\r\n")
#define ERR_NONICKNAMEGIVEN(HOST , USER) (HOST  + " 431 " + USER + " :No nickname given" + "\r\n")
#define ERR_ERRONEUSNICKNAME(HOST , USER) (HOST  + " 432 " + USER + " :Erroneus nickname" + "\r\n")
#define ERR_NICKNAMEINUSE(HOST , USER) (HOST  + " 433 " + USER + " :Nickname is already in use" + "\r\n")
#define ERR_BADCHANNELKEY(HOST , USER) (HOST  + " 475 " + USER + " :Cannot join channel (+k)" + "\r\n")
#define ERR_NOTEXTTOSEND(HOST , USER) (HOST  + " 412 " + USER + " :No text to send" + "\r\n")
#define ERR_CANNOTSENDTOCHAN(HOST , USER) (HOST  + " 404 " + USER + " :Cannot send to channel" + "\r\n")
#define ERR_NOSUCHNICK(HOST , USER, TARG) (HOST  + " 401 " + USER + " " + TARG + " :No such nick/channel" + "\r\n")
#define ERR_NOSUCHSERVER(HOST , USER, TARG) (HOST  + " 402 " + USER + " " + TARG + " :No such server" + "\r\n")
#define ERR_NOSUCHCHANNEL(HOST , USER, TARG) (HOST  + " 403 " + USER + " " + TARG + " :No such channel" + "\r\n")
#define ERR_UNKNOWNCOMMAND(HOST, COMMAND , USER) (HOST + " 421 "+ USER + " " + COMMAND + " :Unknown command" + "\r\n")
#define ERR_USERNOTINCHANNEL(HOST, USER, CHANNEL, TARGET) (HOST  + " 441 " + USER + " " + CHANNEL + " " + TARGET + " :They aren't on that channel" + "\r\n")

#define ERR_CHANOPRIVSNEEDED(HOST, CHANNEL , USER) (HOST  + " 482 " + USER + " " +  CHANNEL + " :You're not channel operator" + "\r\n")
#define ERR_UNKNOWNMODE(HOST , USER, CHANNEL) (HOST  + " 472 " + USER + " " + CHANNEL + " :is unknown mode char to me" + "\r\n")
#define ERR_CHANNELISFULL(HOST , USER, CHANNEL) (HOST  + " 471 " + USER + " " + CHANNEL +  " :Cannot join channel (+l)" + "\r\n")
#define ERR_NOTONCHANNEL(HOST , USER, CHANNEL) (HOST  + " 442 " + USER + " " + CHANNEL +  " :You're not on that channel" + "\r\n")

#define ERR_NOPRIVILEGES(HOST , USER) (HOST  + " 481 " + USER + " :Permission Denied- You're not an IRC operator" + "\r\n")
#define ERR_USERONCHANNEL(HOST , USER, NICK, CHANNEL) (HOST  + " 443 " + USER + " " + CHANNEL + " " + NICK + " :is already on channel" + "\r\n")

#define PRIVMSG(NICK, USER, HOST, COMMAND) (":" + NICK  + "!~" + USER + "@" + HOST + " " + COMMAND + "\r\n")
#define JOIN(NICK, CHANNEL) (":" + NICK  + " JOIN " + CHANNEL + "\r\n")
#define KICK(HOSTMASK, CHANNEL, TARGET, REASON) (":" + HOSTMASK + " KICK " + CHANNEL + " " + TARGET + " " + REASON + "\r\n")
#define PART(HOSTMASK, CHANNEL, REASON) (":" + HOSTMASK + " PART " + CHANNEL + " " + REASON + "\r\n")

class Channels;

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
    std::map<int, User *> &get_clients();
    std::vector<pollfd>& get_fds();
    void binding(std::string);
    std::string get_host();
    int ft_check_auten(std::map<int, User *>& clients, int);
    int ft_get_buffer(std::vector<pollfd> &fd, std::map<int, User *> &clients, int i, std::vector<pollfd>::iterator pollitr);
    int kick_out_client(int socket, std::map<int, User *> &clients, std::vector<pollfd>::iterator it);
    int add_client(std::vector<pollfd> &fds, std::map<int , User *> &clients);
    void Commands(int socket, std::vector<pollfd>::iterator pollitr);
    ~Server();
};

#endif