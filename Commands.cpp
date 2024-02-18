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
    socket = 0;
    if (command.size() != 2)
        send(socket, ERR_NEEDMOREPARAMS(s.get_host(), s.get_clients()[socket]->get_nickname()).c_str(), ERR_NEEDMOREPARAMS(s.get_host(), s.get_clients()[socket]->get_nickname()).size(),0);
    else if (findFromAllChannels(s.get_clients(), command[1]) == s.get_clients().end())
        send(socket, ERR_NOSUCHNICK(s.get_host(), s.get_clients()[socket]->get_nickname(),command[1]).c_str(), ERR_NOSUCHNICK(s.get_host(), s.get_clients()[socket]->get_nickname(), command[1]).size(),0);
    else
    {
        std::map<int , User *>::iterator ite = findFromAllChannels(s.get_clients(), "bot");
        if (ite == s.get_clients().end())
            send(socket, ERR_NOSUCHNICK(s.get_host(), s.get_clients()[socket]->get_nickname(),"bot").c_str(), ERR_NOSUCHNICK(s.get_host(), s.get_clients()[socket]->get_nickname(), "bot").size(),0);
        else
            send(ite->first, (command[1] + "\r\n").c_str(), (command[1] + "\r\n").size(), 0);
    }
}

void Server::Commands(int socket, std::vector<pollfd>::iterator pollitr) {
    std::string str = clients[socket]->get_buffer().substr(0, clients[socket]->get_buffer().find("\r\n"));
    std::vector<Channels>::iterator itrchaine;
    std::map<int, std::string>::iterator useritr;
    std::vector<std::string> command;
    std::stringstream ss(str);
    std::string tmp;
    if (str.empty())
        return ;
    while (getline(ss, tmp, ' '))
        if (!tmp.empty())
            command.push_back(tmp);
    if (command[0] == "NICK")
        nick(command, *this, socket);
    else if (command[0] == "USER")
        return sendError(ERR_ALREADYREGISTERED(this->get_host(), get_clients()[socket]->get_nickname()), socket);
    else if (command[0] == "PASS")
        return sendError(ERR_ALREADYREGISTERED(this->get_host(), get_clients()[socket]->get_nickname()), socket);
    else if (command[0] == "JOIN")
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
    else if (command[0] == "QUIT")
        kick_out_client(socket, get_clients(), pollitr);
    else if (command[0] != "PONG")
    {
        std::string str = ERR_UNKNOWNCOMMAND(get_host(), command[0], clients[socket]->get_nickname());
        send(socket, str.c_str(), str.size(), 0);
    }
}
