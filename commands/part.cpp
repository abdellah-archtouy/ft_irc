#include "../Channels.hpp"

void part(std::vector<std::string> command, Server &s, int socket) {
    if (command.size() < 2)
        return sendError(ERR_NEEDMOREPARAMS(s.get_host(), s.get_clients()[socket]->get_nickname()), socket);
    std::vector<Channels>::iterator itr = findChaine(command[1], s.Channel);
    if (itr == s.Channel.end())
        return sendError(ERR_NOSUCHCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]), socket);
    if (itr->getUsers().find(socket) == itr->getUsers().end())
        return sendError(ERR_NOTONCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]), socket);
    std::string form = FORMA(s.get_clients()[socket]->get_username(), s.get_clients()[socket]->get_nickname(), s.get_clients()[socket]->get_ip());
    std::string reason;
    for (size_t i = 2; i < command.size(); i++)
        reason += command[i] + " ";
    int r = itr->getUsers().size();
    broadCast(*itr, PART(form, itr->getName(), reason));
    itr->getUsers().erase(socket);
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
    if (itr->get_i())
    s.get_clients()[socket]->get_chaine().erase(std::find(s.get_clients()[socket]->get_chaine().begin(),
        s.get_clients()[socket]->get_chaine().end(), itr->getName()));
    if (!howManyMembers(*itr))
        s.Channel.erase(itr);
}