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

void sendMessage(Channels ch, std::string tmp, int socket) {
    std::map<int, std::string>::iterator useritr;
    useritr = ch.getUsers().begin();
    while (useritr != ch.getUsers().end())
    {
        if (useritr->first != socket)
            send(useritr->first, tmp.c_str(), tmp.size(), 0);
        ++useritr;
    }
}

// std::string getCurrentTimestamp() {
//     std::time_t now = std::time(NULL);
//     char timestamp[50];
//     return timestamp;
// }

void sendJoinMessages(Server &s, int socket, Channels& Ch) {
    std::string tmp;
    std::string users;
    std::stringstream ss(s.get_clients()[socket]->get_username());
    getline(ss, tmp, ' ');
    std::string forma = FORMA(tmp, s.get_clients()[socket]->get_nickname(), s.get_clients()[socket]->get_ip());
    send(socket, (":" + forma + " JOIN #" + Ch.getName() + "\r\n").c_str()
        , (":" + forma + " JOIN #" + Ch.getName() + "\r\n").size(), 0);
    std::cout <<":" + forma + " JOIN #" + Ch.getName() + "\r\n" ;
    int i = howManyMembers(Ch);
    if (i == 1)
        Ch.setOper(socket);
    for (std::map<int, std::string>::iterator itr = Ch.getUsers().begin(); itr != Ch.getUsers().end(); ++itr)
        if (std::find(Ch.getOperators().begin(), Ch.getOperators().end(), itr->first) == Ch.getOperators().end())
            users += itr->second + " ";
    for (size_t i = 0; i < Ch.getOperators().size(); i++)
        users += "@" + s.get_clients()[Ch.getOperators()[i]]->get_nickname() + " ";
    if (i == 1)
    {
        std::string str = "MODE #" + Ch.getName() + " +o " + s.get_clients()[socket]->get_nickname() + "\r\n";
        send(socket, str.c_str(), str.size(), 0);
        tmp = "+";
        if (Ch.get_i())
            tmp += "i";
        if (Ch.get_t())
            tmp += "t";
        if (Ch.get_k())
            tmp += "k";
        if (tmp.size() > 1)
            send(socket, (s.get_host() + " MODE #" + Ch.getName() + " " + tmp + "\r\n").c_str(),
                (s.get_host() + " MODE #" + Ch.getName() + " " + tmp + "\r\n").size(), 0);
    }
    if (i > 1)
    {
        if (!Ch.getTopic().empty())
            tmp = RPL_TOPIC(s.get_clients()[socket]->get_nickname(), Ch.getName(), Ch.getTopic());
        else
            tmp = RPL_NOTOPIC(s.get_host(), s.get_clients()[socket]->get_nickname(), Ch.getTopic());
        send(socket, tmp.c_str(), tmp.size(), 0);
        if (!Ch.getTopic().empty())
        {
            tmp = RPL_TOPICWHOTIME(s.get_clients()[socket]->get_nickname(), Ch.getName(), s.get_host(), forma, Ch.get_time());
            send(socket, tmp.c_str(), tmp.size(), 0);
        }
    }
    tmp = RPL_NAMREPLY(s.get_clients()[socket]->get_nickname(), Ch.getName(), " @ ", s.get_host(), users);
    send(socket, tmp.c_str(), tmp.size(), 0);
    tmp = RPL_ENDOFNAMES(s.get_clients()[socket]->get_nickname(), Ch.getName(), s.get_host());
    send(socket, tmp.c_str(), tmp.size(), 0);
    sendMessage(Ch, (forma + " JOIN :#" + Ch.getName() + "\r\n"), socket);
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
            send(socket, ERR_BADCHANNELKEY(s.get_host(), clients[socket]->get_nickname()).c_str(), ERR_BADCHANNELKEY(s.get_host(), clients[socket]->get_nickname()).size(), 0);
            return ;
        }
        if (howManyMembers(*itrchaine) >= itrchaine->getLimit())
        {
            send(socket, ERR_CHANNELISFULL(s.get_host(), clients[socket]->get_nickname(), itrchaine->getName()).c_str(), ERR_CHANNELISFULL(s.get_host(), clients[socket]->get_nickname(), itrchaine->getName()).size(), 0);
            return ;
        }
        itrchaine->setUsers(socket,clients[socket]->get_nickname());
        clients[socket]->set_chaine(itrchaine->getName());
        sendJoinMessages(s, socket, *itrchaine);
    }
    else
        send(socket, ERR_NEEDMOREPARAMS(s.get_host(), clients[socket]->get_nickname()).c_str(), ERR_NEEDMOREPARAMS(s.get_host(), clients[socket]->get_nickname()).size(), 0);
}
