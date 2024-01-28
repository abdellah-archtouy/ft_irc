#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <unistd.h>
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
#include <netinet/in.h>

class User
{
private:
    int _socket;
    bool    is_autho;
    std::string _realname;
    std::string _nikename;
    std::string _passW;
    std::string _user;
    std::string buffer;
    std::string ip_address;
    struct sockaddr_in add;
public:
    User(int socket);
    bool    get_autho_status();
    std::string get_buffer();
    std::string get_ip();
    std::string get_username();
    std::string get_realname();
    std::string get_nickname();
    std::string get_pass();
    struct sockaddr_in get_add();
    void    set_add(struct sockaddr_in);
    void set_autho_status(bool);
    void set_buffer(std::string);
    void set_pass(std::string);
    void set_nickname(std::string);
    void set_username(std::string);
    void set_realname(std::string);
    ~User();
};


#endif