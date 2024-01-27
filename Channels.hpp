#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "Server.hpp"
#include "User.hpp"


class Channels
{
    private:
        std::string ChaName;
        std::string pass;
        std::map<int, std::string> users;
        std::vector<int> operators;
    public:

        // list of operators
        // if someone is not a operator and try to execute a command we must make it impossible
        Channels(std::string ChaName) : ChaName(ChaName) {}
        ~Channels(void) {}
        void setName(std::string name) {
            this->ChaName = name;
        }
        void setPass(std::string pass) {
            this->pass = pass;
        }
        void setUsers(int socket, std::string user) {
            this->users[socket] = user;
        }
        std::string getName() {
            return (this->ChaName);
        }
        std::map<int, std::string>& getUsers() {
            return users;
        }
        std::string getPass() {
            return pass;
        }
        /*  implement Commands as methods    */
};

#endif