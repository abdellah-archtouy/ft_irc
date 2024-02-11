#include "Server.hpp"

void addChannel(std::vector<Channels> &Ch, std::map<std::string, std::string>::iterator itr) {
    Channels c(itr->first);
    if (!itr->second.empty())
    {
        c.setPass(itr->second);
        c.set_k(true);
    }
    Ch.push_back(c);
}

std::vector<Channels>::iterator findChaine(std::string name, std::vector<Channels> &Ch) {
    std::vector<Channels>::iterator it = Ch.end();
    for (size_t i = 0; i < Ch.size(); i++)
    {
        if (Ch[i].getName() == name)
        {
            it = Ch.begin() + i;
            return (it);
        }
    }
    return (it);
}

void bot(std::vector<std::string> command, Server &s, int socket)
{
    if (command.size() != 2)
        send(socket, ERR_NEEDMOREPARAMS(s.get_host(), s.get_clients()[socket]->get_nickname()).c_str(), ERR_NEEDMOREPARAMS(s.get_host(), s.get_clients()[socket]->get_nickname()).size(),0);
    else if (findFromAllChannels(s.get_clients(), command[1]) == s.get_clients().end())
        send(socket, ERR_NOSUCHNICK(s.get_host(), s.get_clients()[socket]->get_nickname(),command[1]).c_str(), ERR_NOSUCHNICK(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]).size(),0);
    else
    {
        std::vector<std::string> command1;
        command1.push_back("PRIVMSG");
        command1.push_back("bot");
        command1.push_back(command[1]);
        privMsg(command1, s, socket);
    }
}

void Server::Commands(int socket) {
    std::string str = clients[socket]->get_buffer().substr(0, clients[socket]->get_buffer().find("\r\n"));
    std::vector<Channels>::iterator itrchaine;
    std::map<int, std::string>::iterator useritr;
    std::vector<std::string> command;
    std::stringstream ss(str);
    std::string tmp;
    if (str.empty())
        return ;
    while (getline(ss, tmp, ' '))
        command.push_back(tmp);
    if (command[0] == "JOIN")
        join(socket, *this, clients, command);
    else if (command[0] == "PRIVMSG")
        privMsg(command, *this, socket);
    else if (command[0] == "MODE")
        mode(command, *this, socket);
    else if (command[0] == "INVITE")
        invite(command, *this, socket);
    else if (command[0] == "TOPIC")
        topic(command, *this, socket);
    else if (command[0] == "KICK")
        kick(command, *this, socket);
    else if (command[0] == "PART")
        part(command, *this, socket);
    else if (command[0] == "BOT")
        bot(command, *this, socket);
}
