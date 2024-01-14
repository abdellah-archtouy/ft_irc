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

class Server
{
private:
    std::vector<pollfd> fds;
    int serversocket;
    std::map<int, User *> clients;
    std::string _pass;
    struct pollfd *fd;
    std::string _port;
public:
    Server(/* args */);
    void polling(void);
    std::map<int, User *> get_clients();
    std::vector<pollfd> get_fds();
    void binding(char **av);
    int ft_check_auten(std::map<int, User *> clients, int);
    ~Server();
};




#endif