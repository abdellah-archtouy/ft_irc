#include "../Channels.hpp"

void sendError(std::string str, int socket)
{
    send(socket, str.c_str(), str.size(), 0);
}

int clientModeArgu(std::vector<std::string> com, int socket, Server &s)
{
    std::string command = com[2];
    if (com.size() != 3)
    {
        sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command), socket);
        return 1;
    }
    if (command[0] != '+' && command[0] != '-')
    {
        sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command), socket);
        return 1;
    }
    for (size_t j = 1; j < command.size(); j++)
    {
        if (command[j] != 'o')
        {
            sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command), socket);
            return 1;
        }
    }
    return 0;
}

int channelModeArgu(std::vector<std::string> com, int socket, Server &s)
{
    std::string command = com[2];
    if (command[0] != '+' && command[0] != '-')
        return (sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command), socket), 1);
    for (size_t j = 1; j < command.size(); j++)
    {
        if (command[j] != 'i' && command[j] != 'k' && command[j] != 't' && command[j] != 'l' && command[j] != 'o') // checked
            return (sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command), socket), 1);
        if (((command[j] == 'k' && command[j] == '+') || command[j] == 'l' || command[j] == 'o') && com.size() != 4) // error
        {
            sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command), socket);
            return 1;
        }
        if (((command[j] == 'k' && command[0] == '-') || command[j] == 'i' || command[j] == 't') && com.size() != 3) // checked
        {
            sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command), socket);
            return 1;
        }
        if (command[j] == 'l' && command[0] == '-')
        {
            sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command), socket);
            return 1;
        }
    }
    return 0;
}

int pareMode(std::vector<std::string> command, int socket, Server &s)
{
    if (findChaine(command[1], s.Channel) == s.Channel.end())
    {
        sendError(ERR_NOSUCHCHANNEL(s.get_host(), s.get_clients()[socket]->get_username(), command[1]), socket);
        return 1;
    }
    if (channelModeArgu(command, socket, s))
        return 1;
    return 0;
}

int parse_key(std::string s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        if ((s[i] >= 48 && s[i] <= 57) ||
            (s[i] >= 65 && s[i] <= 90) ||
            (s[i] >= 97 && s[i] <= 122))
            continue;
        else
            return 1;
    }
    return 0;
}

int parse_limit(std::string s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        if (!isdigit(s[i]))
            return 1;
    }
    return 0;
}

void broadCast(Channels& ch, std::string tmp)
{
    std::map<int, std::string>::iterator useritr;
    useritr = ch.getUsers().begin();
    while (useritr != ch.getUsers().end())
    {
        send(useritr->first, tmp.c_str(), tmp.size(), 0);
        ++useritr;
    }
}

void executeMode(std::vector<std::string> command, int socket, Server &s)
{
    int r;
    std::string str;
    std::vector<Channels>::iterator itr = findChaine(command[1], s.Channel);
    if (itr == s.Channel.end())
    {
        str = ERR_NOSUCHCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]);
        send(socket, str.c_str(), str.size(), 0);
        return ;
    }
    r = 0;
    if (command[2][0] == '+')
        r = 1;
    if (std::find(itr->getOperators().begin(), itr->getOperators().end(), socket) != itr->getOperators().end())
    {
        if (command[2][1] == 'i')
        {
            if (r)
            {
                str = RPL_CHANNELMODEIS(s.get_clients()[socket]->get_nickname(), itr->getName(), s.get_host(), "+i :Channel is invite-only");
                send(socket, str.c_str(), str.size(), 0);
                itr->set_i(true);
            }
            if (!r)
                itr->set_i(false);
        }
        if (command[2][1] == 't')
        {
            if (r)
            {
                str = RPL_CHANNELMODEIS(s.get_clients()[socket]->get_nickname(), itr->getName(), s.get_host(), "+t :Channel topic is set");
                send(socket, str.c_str(), str.size(), 0);
            }
                itr->set_t(true);
            if (!r)
                itr->set_t(false);
        }
        if (command[2][1] == 'k')
        {
            if (r)
            {
                if (!parse_key(command[3]))
                {
                    itr->setPass(command[3]);
                    str = RPL_CHANNELMODEIS(s.get_clients()[socket]->get_nickname(), itr->getName(), s.get_host(), ("+k secret_key :Channel key set to `" + command[3] + "`"));
                    send(socket, str.c_str(), str.size(), 0);
                }
            }
            if (!r)
                itr->setPass("");
        }
        if (command[2][1] == 'l')
        {
            if (r)
            {
                if (!parse_limit(command[3]))
                {
                    itr->set_limit(atoi(command[3].c_str()));
                    itr->set_l(true);
                    str = RPL_CHANNELMODEIS(s.get_clients()[socket]->get_nickname(), itr->getName(), s.get_host(), ("+l user limit :Channel user limit set to `" + command[3] + "`"));
                    sendError(str, socket);
                }
                else
                {
                    sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command[3]), socket);
                    return;
                }
            }
            if (!r)
                itr->set_topic("");
        }
        if (command[2][1] == 'o')
        {
            std::map<int, std::string>::iterator Useritr = findUser(command[3], itr->getUsers());
            if (Useritr == itr->getUsers().end())
            {
                str = ERR_NOSUCHNICK(s.get_host(), s.get_clients()[socket]->get_nickname(), command[3]);
                sendError(str, socket);
                return ;
            }
            if (r)
            {
                itr->setOper(Useritr->first);
                broadCast(*itr, ("MODE " + itr->getName() + " +o " + command[3] + "\r\n"));
            }
            if (!r)
            {
                itr->getOperators().erase(std::find(itr->getOperators().begin(), itr->getOperators().end(), Useritr->first));
                broadCast(*itr, ("MODE " + itr->getName() + " -o " + command[3] + "\r\n"));
            }
        }
    }
    else
        sendError(ERR_CHANOPRIVSNEEDED(s.get_host(), s.get_clients()[socket]->get_nickname(), itr->getName()), socket);
}

void mode(std::vector<std::string> command, Server &s, int socket)
{
    if (pareMode(command, socket, s))
        return;
    executeMode(command, socket, s);
}