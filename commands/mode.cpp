#include "../Channels.hpp"

void sendError(std::string str, int socket) {
    send(socket, str.c_str(), str.size(), 0);
}

int clientModeArgu(std::vector<std::string> com, int socket, Server &s) {
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

int channelModeArgu(std::vector<std::string> com, int socket, Server &s) {
    std::string command = com[2];
    if (command[0] != '+' && command[0] != '-')
    {
        sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command), socket);
        return 1;
    }
    for (size_t j = 1; j < command.size(); j++)
    {
        if (command[j] != 'i' && command[j] != 'k' && command[j] != 't' && command[j] != 'l') // checked
        {
            sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command), socket);
            return 1;
        }
        if (((command[j] == 'k' && command[j] == '+') || command[j] == 'l') && com.size() != 4) // error
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

int pareMode(std::vector<std::string> command, int socket, Server &s) {
    if (command[1][0] == '#')
    {
        if (findChaine(command[1].substr(1, command[1].size()), s.Channel) == s.Channel.end())
        {
            sendError(ERR_NOSUCHCHANNEL(s.get_host(), s.get_clients()[socket]->get_username(), command[1]), socket);
            return 1;
        }
        if (channelModeArgu(command, socket, s))
            return 1;
    }
    else
    {
        if (findFromAllChannels(s.get_clients(), command[1]) == s.get_clients().end())
        {
            sendError(ERR_NOSUCHNICK(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]), socket);
            return 1;
        }
        if (clientModeArgu(command, socket, s))
            return 1;
    }
    return 0;
}

int parse_key(std::string s) {
    for (size_t i = 0; i < s.size(); i++)
    {
        if ((s[i]>=48 && s[i]<=57) ||
            (s[i]>=65 && s[i]<=90) ||
            (s[i]>=97 && s[i]<=122))
            continue;
        else
            return 1;
    }
    return 0;
}

int parse_limit(std::string s) {
    for (size_t i = 0; i < s.size(); i++)
    {
        if (!isdigit(s[i]))
            return 1;
    }
    return 0;
}

void executeMode(std::vector<std::string> command, int socket, Server &s) {
    int r;
    if (command[1][0] == '#')
    {
        std::vector<Channels>::iterator itr = findChaine(command[1].substr(1, command[1].size()), s.Channel);
        for (size_t i = 2; i < command.size(); i++)
        {
            r = 0;
            for (size_t j = 0; j < command[i].size(); j++)
            {
                if (command[i][0] == '+')
                    r = 1;
                if (std::find(itr->getOperators().begin(), itr->getOperators().end(), socket) != itr->getOperators().end())
                {
                    if (command[i][j] == 'i')
                    {
                        if (r)
                           itr->set_i(true);
                        if (!r)
                            itr->set_i(false);
                    }
                    if (command[i][j] == 't')
                    {
                        if (r)
                            itr->set_t(true);
                        if (!r)
                            itr->set_t(false);
                    }
                    if (command[i][j] == 'k')
                    {
                        if (r)
                        {
                            if (parse_key(command[i + 1]))
                                itr->setPass(command[i + 1]);
                            i++;
                        }
                        if (!r)
                            itr->setPass("");
                    }
                    if (command[i][j] == 'l')
                    {
                        if (r)
                        {
                            if (!parse_limit(command[i]))
                                itr->set_limit(atoi(command[i + 1].c_str()));
                            else
                            {
                                sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1].substr(1, command[1].size())), socket);
                                return ;
                            }
                            i++;
                        }
                        if (!r)
                            itr->set_topic("");
                    }
                }
                else
                {
                    if (std::find(itr->getOperators().begin(), itr->getOperators().end(), socket) == itr->getOperators().end())
                        sendError(ERR_NOPRIVILEGES(s.get_host(), s.get_clients()[socket]->get_nickname()), socket);
                    return ;
                }
            }
        }
    }
    else
    {
        std::map<int, User*>::iterator Useritr = findFromAllChannels(s.get_clients(), command[1]);
        std::vector<Channels>::iterator itr = findChaine(s.get_clients()[socket]->get_chaine(), s.Channel);
        for (size_t i = 2; i < command.size(); i++)
        {
            r = 0;
            if (command[i][0] == '+')
                r = 1;
            for (size_t j = 1; j < command[i].size(); j++)
            {
                if (s.get_clients()[Useritr->first]->get_chaine() == s.get_clients()[socket]->get_chaine()
                    && std::find(itr->getOperators().begin(), itr->getOperators().end(), socket) != itr->getOperators().end())
                {
                    if (command[i][j] == 'o')
                    {
                        if (r)
                            itr->setOper(Useritr->first);
                        if (!r)
                            itr->getOperators().erase(std::find(itr->getOperators().begin(), itr->getOperators().end(), Useritr->first));
                    }
                }
                else
                {
                    if (std::find(itr->getOperators().begin(), itr->getOperators().end(), socket) == itr->getOperators().end())
                        sendError(ERR_NOPRIVILEGES(s.get_host(), s.get_clients()[socket]->get_nickname()), socket);
                    return ;
                }
            }
        }
    }
}

void mode(std::vector<std::string>command, Server &s, int socket) {
    if (pareMode(command, socket, s))
        return;
    executeMode(command, socket, s);
}