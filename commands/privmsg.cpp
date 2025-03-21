#include "../Channels.hpp"

std::map<int, User *>::iterator findFromAllChannels(std::map<int, User *> &clients, std::string nickname) {
    std::map<int, User *>::iterator uItr = clients.begin();
    while (uItr != clients.end())
    {
        if (uItr->second->get_nickname() == nickname)
            return (uItr);
        ++uItr;
    }
    return (uItr);
}

int privMsgParsing(std::vector<std::string> param, Server &s, int socket, std::string &message) {
    std::string str;
    if (param.size() < 3)
    {
        if (param.size() == 1)
            str = ERR_CANNOTSENDTOCHAN(s.get_host(), s.get_clients()[socket]->get_username());
        else if (param.size() == 2)
            str = ERR_NOTEXTTOSEND(s.get_host(), s.get_clients()[socket]->get_username());
        send(socket, str.c_str(), str.size(), 0);
        return 1;
    }
    if (param[1][0] == '#')
    {
        std::vector<Channels>::iterator itr = findChaine(param[1], s.Channel);
        if (itr == s.Channel.end())
        {
            str = ERR_NOSUCHCHANNEL(s.get_host(), s.get_clients()[socket]->get_username(), param[1]);
            send(socket, str.c_str(), str.size(), 0);
            return 1;
        }
    }
    else
    {
        std::map<int, User *>::iterator userItr = findFromAllChannels(s.get_clients(), param[1]);
        if (userItr == s.get_clients().end())
        {
            str = ERR_NOSUCHNICK(s.get_host(), s.get_clients()[socket]->get_nickname(), param[1]);
            send(socket, str.c_str(), str.size(), 0);
            return 1;
        }
    }
    message = param[0] + " " + param[1] + " ";
    size_t i = 2;
    for (i = 2; i < param.size(); i++)
        if (param[i][0] == ':')
            break ;
    if (i == param.size())
    {
        message += ":";
        i--;
    }
    while (i < param.size())
    {
        message += param[i] + " ";
        i++;
    }
    return 0;
}

void privMsg(std::vector<std::string> param, Server &s, int socket) {
    std::map<int, std::string>::iterator userInChItr;
    std::map<int, User *>::iterator userItr;
    std::string str ;
    std::string tmp ;
    std::string message ;

    if (privMsgParsing(param, s, socket, str))
        return ;
    message = PRIVMSG(s.get_clients()[socket]->get_nickname(), s.get_clients()[socket]->get_username(), s.get_clients()[socket]->get_ip(), str);
    if (param[1][0] == '#')
    {
        std::vector<Channels>::iterator itr = findChaine(param[1], s.Channel);
        if (std::find(s.get_clients()[socket]->get_chaine().begin(), s.get_clients()[socket]->get_chaine().end(), itr->getName())
            != s.get_clients()[socket]->get_chaine().end())
        {
            for (userInChItr = itr->getUsers().begin(); userInChItr != itr->getUsers().end(); ++userInChItr)
                if (userInChItr->first != socket)
                    send(userInChItr->first, message.c_str(), message.size(), 0);
        }
        else
            return sendError(ERR_NOTONCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), param[1]), socket);
    }
    else
    {
        userItr = findFromAllChannels(s.get_clients(), param[1]);
        if (userItr != s.get_clients().end())
            send(userItr->first, message.c_str(), message.size(), 0);
    }
}