#include "../Channels.hpp"

void invite(std::vector<std::string> command, Server &s, int socket) {
    if (command.size() != 3)
        return sendError(ERR_UNKNOWNCOMMAND(s.get_host(), command[0], s.get_clients()[socket]->get_nickname()), socket);

    std::vector<Channels>::iterator itrChaine = findChaine(command[2], s.Channel);
    if (itrChaine == s.Channel.end())
        return sendError(ERR_NOSUCHCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[2]), socket);
    if (itrChaine->get_i() && std::find(itrChaine->getOperators().begin(), itrChaine->getOperators().end(), socket) == itrChaine->getOperators().end())
        return sendError(ERR_CHANOPRIVSNEEDED(s.get_host(), command[2], s.get_clients()[socket]->get_nickname()), socket);
        
    std::map<int, std::string>::iterator itrUser = itrChaine->getUsers().find(socket);
    if (!itrChaine->get_i() && itrUser == itrChaine->getUsers().end())
        return sendError(ERR_NOTONCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]), socket);

    std::map<int, User *>::iterator user = findFromAllChannels(s.get_clients(), command[1]);
    itrUser = itrChaine->getUsers().find(user->first);
    if (itrUser != itrChaine->getUsers().end())
        return sendError(ERR_USERONCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1], command[2]), socket);

    itrChaine->setUsers(user->first, user->second->get_nickname());
    itrChaine->set_inviteList(user->first); // here we add socket to invite list
    std::string str = ":" + FORMA(s.get_clients()[socket]->get_username(), s.get_clients()[socket]->get_nickname(), s.get_clients()[socket]->get_ip())
        + " 341 " + user->second->get_nickname() + " " + s.get_clients()[socket]->get_nickname() + " " + itrChaine->getName() + "\r\n";
    send(socket, str.c_str(), str.size(), 0);
    str = ":" + FORMA(s.get_clients()[socket]->get_username(), s.get_clients()[socket]->get_nickname(), s.get_clients()[socket]->get_ip())
        + " INVITE " + user->second->get_nickname() + " " + itrChaine->getName() + "\r\n";
    send(user->first, str.c_str(), str.size(), 0);
}