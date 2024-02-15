#include "../Channels.hpp"

void topic(std::vector<std::string> command, Server &s, int socket) {
    if (command.size() < 2)
        return sendError(ERR_UNKNOWNCOMMAND(command[0], s.get_clients()[socket]->get_nickname()), socket);
    
    std::vector<Channels>::iterator itr = findChaine(command[1], s.Channel);
    if (itr == s.Channel.end())
        return sendError(ERR_NOSUCHCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]), socket);
    if (itr->getUsers().find(socket) == itr->getUsers().end())
        return sendError(ERR_NOTONCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]), socket);
    if (itr->get_t() && std::find(itr->getOperators().begin(), itr->getOperators().end(), socket) == itr->getOperators().end() && command.size() > 2)
        return sendError(ERR_CHANOPRIVSNEEDED(s.get_host(), command[1], s.get_clients()[socket]->get_nickname()), socket);
    
    std::string forma = FORMA(s.get_clients()[socket]->get_username(), s.get_clients()[socket]->get_nickname(), s.get_clients()[socket]->get_ip());
    if (command.size() == 2) // announce the topic
    {
        if (itr->getTopic().empty())
            return sendError(RPL_NOTOPIC(s.get_host(), s.get_clients()[socket]->get_nickname(), itr->getName()), socket);
        else
        {
            sendError(RPL_TOPIC(forma, s.get_clients()[socket]->get_nickname(), itr->getName(), itr->getTopic()), socket);
            std::string str;
            ft_putnbr(itr->get_timestamp(), str);
            return sendError(RPL_TOPICWHOTIME(s.get_clients()[socket]->get_nickname(), itr->getName(), s.get_host(), itr->get_topicSetter(), str), socket);
        }
    }
    if (command[2][0] != ':' && command.size() > 3) // here we set the topic we may clear it or gives it a value
        return sendError(ERR_NEEDMOREPARAMS(s.get_host(), s.get_clients()[socket]->get_nickname()), socket);
    
    std::string topic = command[2];
    if (command[2][0] == ':')
        topic = command[2].substr(1, command[2].size()) + " ";
    for (size_t i = 3; i < command.size(); i++)
        topic += command[i] + " ";
    if (topic.size() == 2 && topic[0] == ':')
        itr->set_topic("");
    else
    {
        itr->set_topic(topic);
        itr->set_topicSetter(s.get_clients()[socket]->get_nickname());
        itr->set_timestamp(std::time(NULL));
    }
    std::string message = ":" + forma + " TOPIC " + itr->getName() + " :" + itr->getTopic() + "\r\n";
    broadCast(*itr, message);
}