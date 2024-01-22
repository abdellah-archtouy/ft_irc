#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "Server.hpp"
#include "User.hpp"


class Channels
{
    private:
        std::string ChaName;
        std::vector<int> users;
        std::vector<int> operators;
    public:

        // list of operators
        // if someone is not a operator and try to execute a command we must make it impossible
        Channels(std::string ChaName) : ChaName(ChaName) {}
        ~Channels(void) {}
        void setName(std::string name) {
            this->ChaName = name;
        }
        void setUsers(int user) {
            this->users.push_back(user);
        }
        std::string getName() {
            return (this->ChaName);
        }
        std::vector<int>& getUsers() {
            return users;
        }
        /*  implement Commands as methods    */
};

#endif