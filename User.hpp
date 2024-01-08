#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <unistd.h>

class User
{
private:
    int _socket;
    bool    is_autho;
    std::string _name;
    std::string _nikename;
    std::string _passW;
    std::string buffer;
public:
    User(int socket);
    bool    get_autho_status();
    std::string get_buffer();
    std::string get_name();
    std::string get_nikename();
    std::string get_pass();
    void set_autho_status(bool);
    void set_buffer(std::string);
    void set_pass(std::string);
    void set_nikename(std::string);
    void set_name(std::string);
    ~User();
};


#endif