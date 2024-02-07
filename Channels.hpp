#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "Server.hpp"
#include "User.hpp"

class Server;

class Channels
{
    private:
        std::string ChaName;
        std::string time;
        bool i;
        bool t;
        bool key;
        int limit;
        std::string pass;
        std::string Topic;
        std::map<int, std::string> users;
        std::vector<int> operators;
    public:
        Channels(std::string ChaName) : ChaName(ChaName) , i(false), t(false), key(false), limit(10) {}
        ~Channels(void) {}

        void setName(std::string name) {
            this->ChaName = name;
        }
        void setPass(std::string pass) { this->pass = pass; }
        void setUsers(int socket, std::string user) { this->users[socket] = user; }
        void setOper(int socket) { this->operators.push_back(socket); }
        void set_i(bool i) { this->i = i; }
        void set_k(bool i) { this->key = i; }
        void set_t(bool i) { this->t = i; }
        void set_topic(std::string str) { this->Topic = str; }
        void set_limit(int l) { this->limit = l; }

        bool get_t() { return (this->t); }
        bool get_i() { return (this->i); }
        bool get_k() { return (this->key); }
        std::string get_time() { return (this->time); }
        std::string getTopic() { return (this->Topic); }
        int getLimit() { return (this->limit); }
        std::string getName() { return (this->ChaName); }
        std::vector<int> &getOperators() { return (this->operators); }
        std::map<int, std::string>& getUsers() { return users; }
        std::string getPass() { return pass; }
};

void join(int socket, Server& s, std::map<int , User *> &clients, std::vector<std::string> &command);
void privMsg(std::vector<std::string> param, Server &s, int socket);
void mode(std::vector<std::string>command, Server &s, int socket);
void invite(std::vector<std::string> command, Server &s, int socket);
std::vector<Channels>::iterator findChaine(std::string name, std::vector<Channels> &Ch);
std::map<int, User *>::iterator findFromAllChannels(std::map<int, User *> &clients, std::string nickname);
void addChannel(std::string name, std::vector<Channels> &Ch, std::vector<std::string>& pass);
int parse_limit(std::string s);
int parse_key(std::string s);
void sendMessage(Channels ch, std::string tmp, int socket);
void broadCast(Channels ch, std::string tmp);
void sendError(std::string str, int socket);
void sendJoinMessages(Server &s, int socket, Channels& Ch);
void topic(std::vector<std::string> command, Server &s, int socket);
// std::string getCurrentTimestamp();

#endif