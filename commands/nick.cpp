#include "../Channels.hpp"

void nick(std::vector<std::string> command, Server &s, int socket) {
    if (command.size() < 2)
    {
        send(socket, ERR_NONICKNAMEGIVEN(s.get_host(), s.get_clients()[socket]->get_username()).c_str(), ERR_NONICKNAMEGIVEN(s.get_host(), s.get_clients()[socket]->get_username()).size(), 0);
        return ;
    }
    if (ft_check_nick(s.get_clients(), command[1]) == 2)
        send(socket, ERR_ERRONEUSNICKNAME(s.get_host(), s.get_clients()[socket]->get_username()).c_str(), ERR_ERRONEUSNICKNAME(s.get_host(), s.get_clients()[socket]->get_username()).size(), 0);
    else if (ft_check_nick(s.get_clients(), command[1]))
        send(socket, ERR_NICKNAMEINUSE(s.get_host(), s.get_clients()[socket]->get_username()).c_str(), ERR_NICKNAMEINUSE(s.get_host(), s.get_clients()[socket]->get_username()).size(), 0);
    else
    {
        std::string str1 = s.get_clients()[socket]->get_nickname();
        s.get_clients()[socket]->set_nickname(command[1]);
        std::string str = ":" + FORMA(s.get_clients()[socket]->get_username(), str1, s.get_clients()[socket]->get_ip()) + " NICK :" + s.get_clients()[socket]->get_nickname() + "\r\n";
        for (size_t i = 0; i < s.get_clients()[socket]->get_chaine().size(); i++)
            broadCast((*findChaine(s.get_clients()[socket]->get_chaine()[i], s.Channel)), str); // broadcast to all channels
    }
}