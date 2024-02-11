#include "../Channels.hpp"

void kick(std::vector<std::string> command, Server &s, int socket) {
    if (command.size() < 2)
        return sendError(ERR_NEEDMOREPARAMS(s.get_host(), s.get_clients()[socket]->get_nickname()), socket);
    std::vector<Channels>::iterator itr = findChaine(command[1], s.Channel);
    if (itr == s.Channel.end())
        return sendError(ERR_NOSUCHCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]), socket);
    if (itr->getUsers().find(socket) == itr->getUsers().end())
        return sendError(ERR_NOTONCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]), socket);
    std::map<int, std::string>::iterator useritr = findUser(command[2], itr->getUsers());
    if (useritr == itr->getUsers().end())
        return sendError(ERR_USERNOTINCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1], command[2]), socket);
    if (std::find(itr->getOperators().begin(), itr->getOperators().end(), socket) == itr->getOperators().end())
        return sendError(ERR_CHANOPRIVSNEEDED(s.get_host(), command[1], s.get_clients()[socket]->get_nickname()), socket);
    std::string form = FORMA(s.get_clients()[socket]->get_username(), s.get_clients()[socket]->get_nickname(), s.get_clients()[socket]->get_ip());
    std::string reason;
    for (size_t i = 3; i < command.size(); i++)
        reason += command[i] + " ";
    broadCast(*itr, KICK(form, itr->getName(), command[2], reason));
    std::map<int, User *>::iterator it = findFromAllChannels(s.get_clients(), command[2]);
    it->second->get_chaine().erase(std::find(it->second->get_chaine().begin(), it->second->get_chaine().end(), itr->getName()));
    int r = itr->getUsers().size();
    itr->getUsers().erase(useritr->first);
    if (std::find(itr->getOperators().begin(), itr->getOperators().end(), socket) != itr->getOperators().end())
    {
        if (itr->getOperators().size() == 1 && r > 1)
            {
                itr->setOper(itr->getUsers().begin()->first);
                std::string str = "MODE " + itr->getName() + " +o " + itr->getUsers().begin()->second + "\r\n";
                broadCast(*itr, str);
            }
        itr->getOperators().erase(std::find(itr->getOperators().begin(), itr->getOperators().end(), socket));
    }
}