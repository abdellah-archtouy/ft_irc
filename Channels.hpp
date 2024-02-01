#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "Server.hpp"
#include "User.hpp"

class Server;

class Channels
{
    private:
        std::string ChaName;
        bool i;
        bool t;
        int limit;
        std::string pass;
        std::string Topic;
        std::map<int, std::string> users;
        std::vector<int> operators;
    public:
        // list of operators
        // if someone is not a operator and try to execute a command we must make it impossible
        Channels(std::string ChaName) : ChaName(ChaName) , i(false), t(false), limit(10) {}
        ~Channels(void) {}

        void setName(std::string name) { this->ChaName = name; }
        void setPass(std::string pass) { this->pass = pass; }
        void setUsers(int socket, std::string user) { this->users[socket] = user; }
        void setOper(int socket) { this->operators.push_back(socket); }
        void set_i(bool i) { this->i = i; }
        void set_t(bool i) { this->t = i; }
        void set_topic(std::string str) { this->Topic = str; }
        void set_limit(int l) { this->limit = l; }

        bool get_t() { return (this->t); }
        bool get_i() { return (this->i); }
        std::string getTopic() { return (this->Topic); }
        int getLimit() { return (this->limit); }
        std::string getName() { return (this->ChaName); }
        std::vector<int> &getOperators() { return (this->operators); }
        std::map<int, std::string>& getUsers() { return users; }
        std::string getPass() { return pass; }
        /*  implement Commands as methods    */
};

void join(int socket, Server& s, std::map<int , User *> &clients, std::vector<std::string> &command);
void addChannel(std::string name, std::vector<Channels> &Ch, std::vector<std::string>& pass);
std::vector<Channels>::iterator findChaine(std::string name, std::vector<Channels> &Ch);
void privMsg(std::vector<std::string> param, Server &s, int socket);
void mode(std::vector<std::string>command, Server &s, int socket);
std::map<int, User *>::iterator findFromAllChannels(std::map<int, User *> &clients, std::string nickname);
int parse_limit(std::string s);
int parse_key(std::string s);

#endif