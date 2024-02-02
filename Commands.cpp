#include "Server.hpp"

void addChannel(std::string name, std::vector<Channels> &Ch, std::vector<std::string>& pass) {
    Channels c(name);
    if (pass.size() == 3)
    {
        c.setPass(pass[2]);
        c.set_k(true);
    }
    Ch.push_back(c);
}

std::vector<Channels>::iterator findChaine(std::string name, std::vector<Channels> &Ch) {
    std::vector<Channels>::iterator it = Ch.end();
    for (size_t i = 0; i < Ch.size(); i++)
    {
        if (Ch[i].getName() == name)
        {
            it = Ch.begin() + i;
            return (it);
        }
    }
    return (it);
}

void Server::Commands(int socket) {
    std::string str = clients[socket]->get_buffer().substr(0, clients[socket]->get_buffer().find("\r\n"));
    std::vector<Channels>::iterator itrchaine;
    std::map<int, std::string>::iterator useritr;
    std::vector<std::string> command;
    std::stringstream ss(str);
    std::string tmp;
    if (str.empty())
        return ;
    while (getline(ss, tmp, ' '))
        command.push_back(tmp);
    if (command[0] == "JOIN")
        join(socket, *this, clients, command);
    else if (command[0] == "PRIVMSG")
        privMsg(command, *this, socket);
    else if (command[0] == "MODE")
        mode(command, *this, socket);
}

/*ghad idir les command camlin 3ad ghadi nhandliw les messages*/