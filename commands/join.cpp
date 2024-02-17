#include "../Channels.hpp"

int ParseJoin(std::vector<std::string> &command, std::map<std::string, std::string> &ChandKey) {
    if (command.size() < 2 || command.size() > 3)
        return 1;
    std::stringstream ss(command[1]);
    std::string tmp;
    std::vector<std::string> channel;
    std::vector<std::string> key;
    while (getline(ss, tmp, ','))
    {
        channel.push_back(tmp);
        tmp.clear();
    }
    if (command.size() == 3)
    {
        std::stringstream w(command[2]);
        while (getline(w, tmp, ','))
        {
            key.push_back(tmp);
            tmp.clear();
        }
    }
    if (channel.size() < key.size())
        return 1;
    for (size_t i = 0; i < channel.size(); i++)
    {
        if (i < key.size())
            ChandKey[channel[i]] = key[i];
        else
            ChandKey[channel[i]] = "";
    }
    return 0;
}

int howManyMembers(Channels &ch) {
    std::map<int , std::string>::iterator itr = ch.getUsers().begin();
    int i = 0;
    while (itr != ch.getUsers().end())
    {
        i++;
        ++itr;
    }
    return (i);
}

void sendMessage(Channels ch, std::string tmp, int socket) {
    std::map<int, std::string>::iterator useritr;
    useritr = ch.getUsers().begin();
    while (useritr != ch.getUsers().end())
    {
        if (useritr->first != socket)
            send(useritr->first, tmp.c_str(), tmp.size(), 0);
        ++useritr;
    }
}

void	ft_putnbr(std::time_t n, std::string &str)
{
	if (n < 0)
	{
		str += "-";
		n *= -1;
	}
	if (n >= 10)
	{
		ft_putnbr((n / 10), str);
		ft_putnbr((n % 10), str);
	}
	if (n >= 0 && n < 10)
		str += n + 48;
}

void sendJoinMessages(Server &s, int socket, Channels& Ch) {
    std::string tmp;
    std::string users;

    std::string forma = FORMA(s.get_clients()[socket]->get_username(), s.get_clients()[socket]->get_nickname(), s.get_clients()[socket]->get_ip());
    send(socket, (":" + forma + " JOIN " + Ch.getName() + "\r\n").c_str()
        , (":" + forma + " JOIN " + Ch.getName() + "\r\n").size(), 0);
    int i = howManyMembers(Ch);
    if (i == 1)
        Ch.setOper(socket);
    for (std::map<int, std::string>::iterator itr = Ch.getUsers().begin(); itr != Ch.getUsers().end(); ++itr)
        if (std::find(Ch.getOperators().begin(), Ch.getOperators().end(), itr->first) == Ch.getOperators().end())
            users += itr->second + " ";
    size_t j = Ch.getOperators().size();
    for (size_t i = 0; i < j; i++)
            users += "@" + s.get_clients()[Ch.getOperators()[i]]->get_nickname() + " ";
    if (i == 1)
    {
        std::string str = "MODE " + Ch.getName() + " +o " + s.get_clients()[socket]->get_nickname() + "\r\n";
        send(socket, str.c_str(), str.size(), 0);
        tmp = "+";
        if (Ch.get_i())
            tmp += "i";
        if (Ch.get_t())
            tmp += "t";
        if (Ch.get_k())
            tmp += "k";
        str = s.get_host() + " MODE " + Ch.getName() + " " + tmp + "\r\n";
        if (tmp.size() > 1)
            send(socket, str.c_str(), str.size(), 0);
    }
    if (i > 1)
    {
        if (!Ch.getTopic().empty())
            tmp = RPL_TOPIC(forma, s.get_clients()[socket]->get_nickname(), Ch.getName(), Ch.getTopic());
        else
            tmp = RPL_NOTOPIC(s.get_host(), s.get_clients()[socket]->get_nickname(), Ch.getName());
        send(socket, tmp.c_str(), tmp.size(), 0);
        if (!Ch.getTopic().empty())
        {
            std::string str;
            ft_putnbr(Ch.get_timestamp(), str);
            tmp = RPL_TOPICWHOTIME(s.get_clients()[socket]->get_nickname(), Ch.getName(), s.get_host(), Ch.get_topicSetter(), str);
            send(socket, tmp.c_str(), tmp.size(), 0);
        }
    }
    std::string c = " = ";
    tmp = RPL_NAMREPLY(s.get_clients()[socket]->get_nickname(), Ch.getName(), c, s.get_host(), users);
    send(socket, tmp.c_str(), tmp.size(), 0);
    tmp = RPL_ENDOFNAMES(s.get_clients()[socket]->get_nickname(), Ch.getName(), s.get_host());
    send(socket, tmp.c_str(), tmp.size(), 0);
    sendMessage(Ch, (":" + forma + " JOIN :" + Ch.getName() + "\r\n"), socket);
}

void join(int socket, Server& s, std::map<int , User *> &clients, std::vector<std::string> &command) {
    std::vector<Channels>::iterator itrchaine;
    std::map<int, std::string>::iterator useritr;
    std::map<std::string, std::string> ChandKey;
    std::string tmp;
    if (!ParseJoin(command, ChandKey))
    {
        std::map<std::string, std::string>::iterator itr;
        for (itr = ChandKey.begin(); itr != ChandKey.end(); ++itr)
        {
            if (itr->first[0] != '#' || itr->first.size() == 1)
                return sendError(ERR_NEEDMOREPARAMS(s.get_host(), clients[socket]->get_nickname()), socket);
            if (findChaine(itr->first, s.Channel) == s.Channel.end())
                addChannel(s.Channel, itr);
            itrchaine = findChaine(itr->first, s.Channel);
            if (std::find(clients[socket]->get_chaine().begin(), clients[socket]->get_chaine().end(), itrchaine->getName()) != clients[socket]->get_chaine().end()) // here i checked if the client is alread in channel
                return ;
            if (itrchaine->get_i() && std::find(itrchaine->get_inviteList().begin(), itrchaine->get_inviteList().end(), socket) == itrchaine->get_inviteList().end()) // here i see if the client is on invite list in +i channel
            {
                send(socket, ERR_INVITEONLYCHAN(s.get_host(), clients[socket]->get_nickname(), itrchaine->getName()).c_str(),
                    ERR_INVITEONLYCHAN(s.get_host(), clients[socket]->get_nickname(), itrchaine->getName()).size(), 0);
                return ;
            }
            if (!itrchaine->getPass().empty() && ((!itr->second.empty() && itrchaine->getPass() != itr->second) || itr->second.empty())) // check the password if the channel is +k
            {
                send(socket, ERR_BADCHANNELKEY(s.get_host(), clients[socket]->get_nickname()).c_str(), ERR_BADCHANNELKEY(s.get_host(), clients[socket]->get_nickname()).size(), 0);
                return ;
            }
            if (itrchaine->get_l() && howManyMembers(*itrchaine) >= itrchaine->getLimit()) // i check the user limit if i passed the user limites +l
            {
                send(socket, ERR_CHANNELISFULL(s.get_host(), clients[socket]->get_nickname(), itrchaine->getName()).c_str(),
                ERR_CHANNELISFULL(s.get_host(), clients[socket]->get_nickname(), itrchaine->getName()).size(), 0);
                return ;
            }
            itrchaine->setUsers(socket,clients[socket]->get_nickname());
            clients[socket]->set_chaine(itrchaine->getName());
            sendJoinMessages(s, socket, *itrchaine);
            if (itrchaine->get_i() && std::find(itrchaine->get_inviteList().begin(), itrchaine->get_inviteList().end(), socket) != itrchaine->get_inviteList().end()) // erase the joined one from invite list if he(her) join the channel in +i
                itrchaine->get_inviteList().erase(std::find(itrchaine->get_inviteList().begin(), itrchaine->get_inviteList().end(), socket));
        }
    }
    else
        send(socket, ERR_NEEDMOREPARAMS(s.get_host(), clients[socket]->get_nickname()).c_str(), ERR_NEEDMOREPARAMS(s.get_host(), clients[socket]->get_nickname()).size(), 0);
}
