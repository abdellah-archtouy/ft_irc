#include "Server.hpp"

int ParseJoin(std::vector<std::string> &command) {
    if (command.size() < 2 || command.size() > 3)
        return 1;
    if (command[1][0] != '#')
        return 1;
    return 0;
}

void addChannel(std::string name, std::vector<Channels> &Ch, std::vector<std::string>& pass) {
    Channels c(name);
    if (pass.size() == 3)
        c.setPass(pass[2]);
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

std::map<int, std::string>::iterator findUser(std::string name, std::map<int, std::string> users) {
    std::map<int, std::string>::iterator itr;
    for (itr = users.begin(); itr != users.end(); ++itr)
        if (itr->second == name)
            break ;
    return (itr);
}

void join(int socket, Server& s, std::map<int , User *> &clients, std::vector<std::string> &command) {
    std::vector<Channels>::iterator itrchaine;
    std::string tmp;
    if (!ParseJoin(command))
    {
        tmp = command[1].substr(1, command[1].size() - 1);
        if (findChaine(tmp, s.Channel) == s.Channel.end())
            addChannel(tmp, s.Channel, command);
        itrchaine = findChaine(tmp, s.Channel);
        if (!itrchaine->getPass().empty() && itrchaine->getPass() != command[2])
        {
            send(socket, ERR_BADCHANNELKEY(s.get_host(), clients[socket]->get_username()).c_str(), ERR_BADCHANNELKEY(s.get_host(), clients[socket]->get_username()).size(), 0);
            return ;
        }
        itrchaine->setUsers(socket, clients[socket]->get_nickname());
        clients[socket]->set_chaine(itrchaine->getName());
        tmp = ":" + clients[socket]->get_nickname() + " JOIN #" + itrchaine->getName() + "\r\n";
        send(socket, tmp.c_str(), tmp.size(), 0);
    }
    else
        send(socket, ERR_NEEDMOREPARAMS(s.get_host(), clients[socket]->get_username()).c_str(), ERR_NEEDMOREPARAMS(s.get_host(), clients[socket]->get_username()).size(), 0);
}

void privMsg(std::vector<std::string> param, Server &s, int socket) {
    std::map<int, std::string>::iterator userItr;
    std::vector<Channels>::iterator itr;
    std::string str ;
    std::string tmp ;
    std::string message ;

    if (param.size() < 3)
    {
        if (param.size() == 1)
            str = ERR_CANNOTSENDTOCHAN(s.get_host(), s.get_clients()[socket]->get_username());
        else if (param.size() == 2)
            str = ERR_NOTEXTTOSEND(s.get_host(), s.get_clients()[socket]->get_username());
        send(socket, str.c_str(), str.size(), 0);
        return ;
    }

    itr = findChaine(param[1].substr(1, param[1].size()), s.Channel);

    if ((itr != s.Channel.end() && param[1][0] != '#') || param[2][0] != ':')
    {
        str = ERR_CANNOTSENDTOCHAN(s.get_host(), s.get_clients()[socket]->get_username());
        send(socket, str.c_str(), str.size(), 0);
        return ;
    }
    for (size_t i = 0; i < param.size(); i++)
    {
        str += param[i];
        str += " ";
    }
    std::stringstream ss(s.get_clients()[socket]->get_username());//= s.get_clients()[socket]->get_nickname() + ": ";
    getline(ss, tmp, ' ');
    message = PRIVMSG(s.get_clients()[socket]->get_nickname(),
                tmp, "10.11.4.8", str);
    if (itr != s.Channel.end() && itr->getName() == s.get_clients()[socket]->get_chaine())
    {
        for (userItr = itr->getUsers().begin(); userItr != itr->getUsers().end(); ++userItr)
            if (userItr->first != socket)
                send(userItr->first, message.c_str(), message.size(), 0);
    }
    else if (itr == s.Channel.end() && findChaine(s.get_clients()[socket]->get_chaine(), s.Channel) != s.Channel.end())
    {
        itr = findChaine(s.get_clients()[socket]->get_chaine(), s.Channel); // find channel
        userItr = findUser(param[1], itr->getUsers());
        if (userItr != itr->getUsers().end())
            send(userItr->first, message.c_str(), message.size(), 0);
    }
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
    else if (!clients[socket]->get_chaine().empty())
    {
        tmp = clients[socket]->get_nickname() + ": " + str + "\n";
        itrchaine = findChaine(clients[socket]->get_chaine(), this->Channel);
        if (itrchaine != this->Channel.end())
        {
            useritr = itrchaine->getUsers().begin();
            while (useritr != itrchaine->getUsers().end()) {
                if (useritr->first != socket)
                    send(useritr->first , tmp.c_str(), tmp.size(), 0);
                ++useritr;
            }
        }
    }
}

/*ghad idir les command camlin 3ad ghadi nhandliw les messages*/