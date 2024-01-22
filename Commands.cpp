#include "Server.hpp"

int ParseJoin(std::vector<std::string> &command) {
    if (command.size() != 2)
        return 1;
    if (command[1].size() < 2 || command[1][0] != '#')
        return 1;
    return 0;
}

void addChannel(std::string name, std::vector<Channels> &Ch) {
    Channels c(name);
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

void Commands(std::map<int , User *> &clients, int fd, std::vector<Channels> &Ch) {
    std::string str = clients[fd]->get_buffer().substr(0, clients[fd]->get_buffer().find("\r\n"));
    std::vector<Channels>::iterator itrchaine;
    std::vector<std::string> command;
    std::stringstream ss(str);
    std::string tmp;
    (void)Ch;
    if (str.empty())
        return ;
    while (getline(ss, tmp, ' '))
        command.push_back(tmp);
    if (command[0] == "JOIN" && !ParseJoin(command))
    {
        tmp = "\033[1;32mYou Have Been Joined #" + command[1].substr(1, command[1].size()) + " Channel\n";
        send(fd, tmp.c_str(), tmp.size(), 0);
        tmp.clear();
        tmp = command[1].substr(1, command[1].size());
        if (findChaine(tmp, Ch) == Ch.end())
            addChannel(tmp, Ch);
        itrchaine = findChaine(tmp, Ch);
        itrchaine->setUsers(fd);
        clients[fd]->set_chaine(tmp);
    }
    else if (command[0] == "JOIN")
        send(fd, "JOIN command syntax error\n", 26, 0);
    else if (!clients[fd]->get_chaine().empty())
    {
        tmp = clients[fd]->get_nikename() + ": " + str + "\n";
        itrchaine = findChaine(clients[fd]->get_chaine(), Ch);
        for (size_t i = 0; i < itrchaine->getUsers().size(); i++)
            if (itrchaine->getUsers()[i] != fd)
                send(itrchaine->getUsers()[i], tmp.c_str(), tmp.size(), 0);
    }
}

/*ghad idir les command camlin 3ad ghadi nhandliw les messages*/