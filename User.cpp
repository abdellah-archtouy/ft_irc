#include "User.hpp"


User::User(int socket) : _socket(socket) , _realname(""), _nikename(""), _passW("") , _user("*")
{
    this->buffer = "";
    this->is_autho = false;
}

void User::set_realname(std::string name)
{
    _realname = name;
}

void User::set_username(std::string name)
{
    _user = name;
}

void User::set_pass(std::string pass)
{
    _passW = pass;
}

void User::set_nickname(std::string nikename)
{
    _nikename = nikename;
}

void User::set_buffer(std::string buffer1)
{
    if(this->buffer.find("\r\n") != std::string::npos)
    {
        this->buffer.clear();
        this->buffer = buffer1;
    }
    else
    {
        this->buffer = this->buffer + buffer1;
    }
}

void User::set_add(struct sockaddr_in ad)
{
    add = ad;
    char ipAddressBuffer[INET_ADDRSTRLEN];
    const char* result = inet_ntop(AF_INET, &ad.sin_addr.s_addr, ipAddressBuffer, INET_ADDRSTRLEN);
    if (result == 0) {
        std::cerr << "Error converting struct in_addr to string" << std::endl;
        return ;
    }
    char storedIpAddress[INET_ADDRSTRLEN];
    strncpy(storedIpAddress, result, sizeof(storedIpAddress));
    storedIpAddress[sizeof(storedIpAddress) - 1] = '\0';
    this->ip_address = storedIpAddress;
}

struct sockaddr_in User::get_add()
{
    return (add);
}

bool    User::get_autho_status()
{
    return this->is_autho;
}

void    User::set_autho_status(bool autho)
{
    this->is_autho = autho;
}

std::string User::get_buffer()
{
    return(buffer);
}
std::string User::get_ip(){
    return ip_address;
}

std::string User::get_realname()
{
    return (_realname);
}
std::string User::get_username()
{
    return (_user);
}

std::string User::get_nickname()
{
    return (_nikename);
}

std::string User::get_pass()
{
    return (_passW);
}

User::~User()
{
    close(_socket);
}

    // std::cout << "IPv4 Address: " << (ipAddress & 0xFF) << "."
    //           << ((ipAddress >> 8) & 0xFF) << "."
    //           << ((ipAddress >> 16) & 0xFF) << "."
    //           << ((ipAddress >> 24) & 0xFF) << std::endl;