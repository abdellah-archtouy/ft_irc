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
    void binding(char **av);
    int ft_check_auten(std::map<int, User *> clients, int);
    int ft_get_buffer(std::vector<pollfd> &fd, std::map<int , User *> &clients, int i);
    int kick_out_client(std::vector<pollfd> &fds, std::map<int , User *> &clients, int i, std::vector<pollfd>::iterator it);
    int add_client(std::vector<pollfd> &fds, std::map<int , User *> &clients);
    ~Server();
};

void Commands(std::map<int , User *> &clients, int fd, std::vector<Channels> &Channel);


#endif