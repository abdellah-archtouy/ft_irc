#include "../Channels.hpp"

void sendError(std::string str, int socket)
{
    send(socket, str.c_str(), str.size(), 0);
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

int pareMode(std::vector<std::string> command, int socket, Server &s, std::map<std::string, std::string> &mode)
{
    if (command.size() < 3)
        return (sendError(ERR_NEEDMOREPARAMS(s.get_host(), s.get_clients()[socket]->get_nickname()), socket), 1);

    std::vector<std::string> flag;
    std::vector<std::string> argu;
    std::string tmp;
    char c = command[2][0];
    size_t i;
    size_t r = 0;
    if (findChaine(command[1], s.Channel) == s.Channel.end())
    {
        sendError(ERR_NOSUCHCHANNEL(s.get_host(), s.get_clients()[socket]->get_username(), command[1]), socket);
        return 1;
    }
    for (i = 0; i < command[2].size(); i++)
        if (command[2][i] != '+' && command[2][i] != '-')
            break ;
    while (i < command[2].size())
    {
        if (command[2][i] != 't' && command[2][i] != 'l' && command[2][i] != 'k' && command[2][i] != 'o' && command[2][i] != 'i')
            return (sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command[2]), socket), 1);;
        if (c == '+')
            tmp = "+";
        else
            tmp = "-";
        tmp.push_back(command[2][i]);
        flag.push_back(tmp);
        i++;
    }
    for (size_t j = 3; j < command.size(); j++)
        argu.push_back(command[j]);
    if (flag.size() > 4 || argu.size() > 2)
        return (sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command[2]), socket), 1);;
    for (size_t i = 0; i < flag.size(); i++)
    {
        if (strchr(flag[i].c_str(), 'i') || strchr(flag[i].c_str(), 't'))
            r++;
    }
    for (size_t i = 0; i < flag.size(); i++)
    {
        if (c == '+')
        {
            if (flag.size() - r != argu.size())
                return (sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command[2]), socket), 1);
        }
        else
        {
            if (argu.size() && flag[i] != "-o")
                return (sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), command[2]), socket), 1);
        }
    }
    r = 0;
    for (size_t i = 0; i < flag.size(); i++)
    {
        if (flag[i] != "-o" && (strchr(flag[i].c_str(), 'i') || strchr(flag[i].c_str(), 't') || c == '-'))
            mode[flag[i]] = "";
        else
        {
            mode[flag[i]] = argu[r];
            r++;
        }
    }
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

void executeMode(std::map<std::string, std::string> map, int socket, Server &s, Channels &Ch)
{
    if (std::find(Ch.getOperators().begin(), Ch.getOperators().end(), socket) != Ch.getOperators().end())
    {
       std::map<std::string, std::string>::iterator itr;
       std::string str;
        for (itr = map.begin(); itr != map.end() ; ++itr)
        {
            if (itr->first == "+i")
            {
                if (Ch.get_i())
                    return ;
                Ch.set_i(true);
                str = s.get_host() + " MODE " + Ch.getName() + " +i\r\n";
            }
            if (itr->first == "-i")
            {
                if (!Ch.get_i())
                    return ;
                Ch.set_i(false);
                str = s.get_host() + " MODE " + Ch.getName() + " -i\r\n";
            }
            if (itr->first == "+l")
            {
                if (Ch.get_l())
                    return ;
                if (!parse_limit(itr->second))
                {
                    Ch.set_limit(atoi(itr->second.c_str()));
                    Ch.set_l(true);
                    str = s.get_host() + " MODE " + Ch.getName() + " +l " + itr->second + "\r\n";
                }
                else
                    return sendError(ERR_UNKNOWNMODE(s.get_host(), s.get_clients()[socket]->get_nickname(), itr->second), socket);
            }
            if (itr->first == "-l")
            {
                if (!Ch.get_l())
                    return ;
                Ch.set_l(false);
                str = s.get_host() + " MODE " + Ch.getName() + " -l\r\n";
            }
            if (itr->first == "+t")
            {
                if (Ch.get_t())
                    return ;
                Ch.set_t(true);
                str = s.get_host() + " MODE " + Ch.getName() + " +t\r\n";
            }
            if (itr->first == "-t")
            {
                if (!Ch.get_t())
                    return ;
                Ch.set_t(false);
                str = s.get_host() + " MODE " + Ch.getName() + " -t\r\n";
            }
            if (itr->first == "+k")
            {
                if (Ch.get_k())
                    return ;
                if (!parse_key(itr->second))
                {
                    Ch.setPass(itr->second);
                    Ch.set_k(true);
                    str = s.get_host() + " MODE " + Ch.getName() + " +k " + itr->second + "\r\n";
                }
            }
            if (itr->first == "-k")
            {
                if (!Ch.get_k())
                    return ;
                Ch.setPass("");
                Ch.set_k(false);
                str = s.get_host() + " MODE " + Ch.getName() + " -k\r\n";
            }
            if (itr->first[1] == 'o')
            {
                std::map<int, std::string>::iterator Useritr = findUser(itr->second, Ch.getUsers());
                if (Useritr == Ch.getUsers().end())
                {
                    str = ERR_NOSUCHNICK(s.get_host(), s.get_clients()[socket]->get_nickname(), itr->second);
                    sendError(str, socket);
                    return ;
                }
                if (itr->first == "+o")
                {
                    if (std::find(Ch.getOperators().begin(), Ch.getOperators().end(), Useritr->first) != Ch.getOperators().end())
                        return ;
                    Ch.setOper(Useritr->first);
                    str = (s.get_host() + " MODE " + Ch.getName() + " +o " + itr->second + "\r\n");
                }
                if (itr->first == "-o")
                {
                    if (std::find(Ch.getOperators().begin(), Ch.getOperators().end(), Useritr->first) == Ch.getOperators().end())
                        return ;
                    Ch.getOperators().erase(std::find(Ch.getOperators().begin(), Ch.getOperators().end(), Useritr->first));
                    str = (s.get_host() + " MODE " + Ch.getName() + " -o\r\n");
                }
            }
            broadCast(Ch, str);
        }
    }
    else
        sendError(ERR_CHANOPRIVSNEEDED(s.get_host(), s.get_clients()[socket]->get_nickname(), Ch.getName()), socket);
}

void mode(std::vector<std::string> command, Server &s, int socket)
{
    std::map<std::string, std::string> map;
    if (pareMode(command, socket, s, map))
        return;
    std::string str;
    std::vector<Channels>::iterator itrChan = findChaine(command[1], s.Channel);
    if (itrChan == s.Channel.end())
    {
        str = ERR_NOSUCHCHANNEL(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]);
        send(socket, str.c_str(), str.size(), 0);
        return ;
    }
    executeMode(map, socket, s, *itrChan);
}