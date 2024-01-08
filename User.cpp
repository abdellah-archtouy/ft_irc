#include "User.hpp"

User::User(int socket) : _socket(socket) , _name(""), _nikename(""), _passW("")
{
    this->buffer = "";
    this->is_autho = false;
}

void User::set_name(std::string name)
{
    _name = name;
}

void User::set_pass(std::string pass)
{
    _passW = pass;
}

void User::set_nikename(std::string nikename)
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

std::string User::get_name()
{
    return (_name);
}

std::string User::get_nikename()
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
