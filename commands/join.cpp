#include "../Channels.hpp"

int ParseJoin(std::vector<std::string> &command) {
    if (command.size() < 2 || command.size() > 3)
        return 1;
    if (command[1][0] != '#')
        return 1;
    return 0;
}

int howManyMembers(Channels &ch) {
    std::map<int , std::string>::iterator itr = ch.getUsers().begin();
    int i = 0;
    while (itr != ch.getUsers().end())
    {
        i++;
        ++itr;
    }
    return (i);
}

void join(int socket, Server& s, std::map<int , User *> &clients, std::vector<std::string> &command) {
    std::vector<Channels>::iterator itrchaine;
    std::map<int, std::string>::iterator useritr;
    std::string tmp;
    if (!ParseJoin(command))
    {
        tmp = command[1].substr(1, command[1].size() - 1);
        if (findChaine(tmp, s.Channel) == s.Channel.end())
            addChannel(tmp, s.Channel, command);
        itrchaine = findChaine(tmp, s.Channel);
        if (itrchaine->getName() == s.get_clients()[socket]->get_chaine())
            return ;
        if (!itrchaine->getPass().empty() && itrchaine->getPass() != command[2])
        {
            send(socket, ERR_BADCHANNELKEY(s.get_host(), clients[socket]->get_username()).c_str(), ERR_BADCHANNELKEY(s.get_host(), clients[socket]->get_username()).size(), 0);
            return ;
        }
        if (!howManyMembers(*itrchaine))
        {
            itrchaine->setOper(socket);
            std::string str = "MODE #" + itrchaine->getName() + " +o " + clients[socket]->get_nickname() + "\r\n";
            std::cout << str << std::endl;
            send(socket, str.c_str(), str.size(), 0);
        }
        if (howManyMembers(*itrchaine) >= itrchaine->getLimit())
        {
            send(socket, ERR_CHANNELISFULL(s.get_host(), clients[socket]->get_username(), itrchaine->getName()).c_str(), ERR_CHANNELISFULL(s.get_host(), clients[socket]->get_username(), itrchaine->getName()).size(), 0);
            return ;
        }
        itrchaine->setUsers(socket,clients[socket]->get_nickname());
        clients[socket]->set_chaine(itrchaine->getName());
        tmp = JOIN(s.get_clients()[socket]->get_nickname(), itrchaine->getName());
        std::cout << tmp << std::endl;
        useritr = itrchaine->getUsers().begin();
        while (useritr != itrchaine->getUsers().end())
        {
            send(useritr->first, tmp.c_str(), tmp.size(), 0);
            ++useritr;
        }
    }
    else
        send(socket, ERR_NEEDMOREPARAMS(s.get_host(), clients[socket]->get_username()).c_str(), ERR_NEEDMOREPARAMS(s.get_host(), clients[socket]->get_username()).size(), 0);
}
