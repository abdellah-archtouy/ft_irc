#include "Server.hpp"

Server::Server()
{
    serversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serversocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    int reuseAddr = 1;
    if (setsockopt(serversocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1)
    {
        std::cerr << "Error setting SO_REUSEADDR option" << std::endl;
        close(serversocket);
        exit(EXIT_FAILURE);
    }
}

int ft_check_param(std::vector<std::string> parameters)
{
    std::string port = parameters[0];
    for (size_t i = 0; i < port.size(); i++)
    {
        if (!isdigit(port[i]))
            return 1;
    }
    int a = atoi(port.c_str());
    if (a < 1024 || a > 65535)
        return 1;
    return (0);
}

std::string Server::get_host()
{
    char hostname[256];
    std::string host = ":";
    if (gethostname(hostname, sizeof(hostname)) == 0)
        host = host + hostname;
    return host;
}

void Server::binding(std::string line)
{
    std::stringstream split(line);
    std::vector<std::string> parameters;
    std::string param;
    while (split >> param)
    {
        parameters.push_back(param);
    }
    if (parameters.size() != 2 || ft_check_param(parameters))
    {
        std::cout << "parameters Error" << std::endl;
        exit(1);
    }
    _port = parameters[0];
    _pass = parameters[1];
    sockaddr_in ServerAddress;
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    ServerAddress.sin_port = htons(atoi(_port.c_str()));

    if (bind(serversocket, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) == -1)
    {
        std::cerr << "Error binding socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(serversocket, 50) == -1)
    {
        std::cerr << "Error listening for connections" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int ft_check_nick(std::map<int, User *> client, std::string nick)
{
    std::map<int, User *>::iterator it = client.begin();

    if (isdigit(nick[0]))
        return 2;
    size_t i = 0;
    while (i < nick.size())
    {
        if (nick[i] == ':' || nick[i] == '#' || nick[i] == '&' || isspace(nick[i]))
            return 2;
        i++;
    }
    while (it != client.end())
    {
        if (it->second->get_nickname() == nick)
            return 1;
        it++;
    }
    return 0;
}

int ft_chek_user(std::string user)
{
    std::stringstream a(user);
    (void)user;
    std::string word;
    a >> word;
    a >> word;
    if (word != "0")
        return (1);
    a >> word;
    if (word != "*")
        return (1);
    a >> word;
    if (word == "*")
        return (1);
    return 0;
}
std::string ft_get_username(std::string user)
{
    std::stringstream a(user);
    (void)user;
    std::string word;
    a >> word;
    return word;
}
std::string ft_get_realname(std::string user)
{
    std::stringstream a(user);
    (void)user;
    std::string word;
    a >> word;
    a >> word;
    a >> word;
    a >> word;
    return word;
}

int Server::ft_check_auten(std::map<int, User *>& client, int socket)
{
    std::string buffer = client[socket]->get_buffer().substr(0, client[socket]->get_buffer().find("\r\n"));
    std::stringstream split(buffer);
    std::string word;
    std::vector<std::string> command;
    std::getline(split, word, ' ');
    while (word != "")
    {
        command.push_back(word);
        word.clear();
        std::getline(split, word);
    }
    if (command[0] != "PASS" && command[0] != "NICK" && command[0] != "USER")
        return send(socket, ERR_NOTREGISTERED(get_host(), clients[socket]->get_username()).c_str(), ERR_NOTREGISTERED(get_host(), clients[socket]->get_username()).size(), 0), 1;
    if (command[0] == "PASS")
    {
        if (command.size() < 2)
            return send(socket, ERR_NEEDMOREPARAMS(get_host(), clients[socket]->get_username()).c_str(), ERR_NEEDMOREPARAMS(get_host(), clients[socket]->get_username()).size(), 0), 1;
        if (client[socket]->get_pass() != "")
            send(socket, ERR_ALREADYREGISTERED(get_host(), clients[socket]->get_username()).c_str(), ERR_ALREADYREGISTERED(get_host(), clients[socket]->get_username()).size(), 0);
        else if (command[1] == this->_pass)
            client[socket]->set_pass(command[1]);
        else
            send(socket, ERR_PASSWDMISMATCH(get_host(), clients[socket]->get_username()).c_str(), ERR_PASSWDMISMATCH(get_host(), clients[socket]->get_username()).size(), 0);
    }
    else if (command[0] == "USER")
    {
        if (client[socket]->get_username() != "*")
            send(socket, ERR_ALREADYREGISTERED(get_host(), clients[socket]->get_username()).c_str(), ERR_NOTREGISTERED(get_host(), clients[socket]->get_username()).size(), 0);
        else if (ft_chek_user(command[1]))
            send(socket, ERR_NEEDMOREPARAMS(get_host(), clients[socket]->get_username()).c_str(), ERR_NEEDMOREPARAMS(get_host(), clients[socket]->get_username()).size(), 0);
        else
        {
            client[socket]->set_username(ft_get_username(command[1]));
            client[socket]->set_realname(ft_get_realname(command[1]));
        }
    }
    else if (command[0] == "NICK")
    {
        if (command.size() < 2)
            return send(socket, ERR_NONICKNAMEGIVEN(get_host(), clients[socket]->get_username()).c_str(), ERR_NONICKNAMEGIVEN(get_host(), clients[socket]->get_username()).size(), 0), 1;
        if (client[socket]->get_nickname() != "")
            send(socket, ERR_ALREADYREGISTERED(get_host(), clients[socket]->get_username()).c_str(), ERR_ALREADYREGISTERED(get_host(), clients[socket]->get_username()).size(), 0);
        else if (ft_check_nick(clients, command[1]) == 2)
            send(socket, ERR_ERRONEUSNICKNAME(get_host(), clients[socket]->get_username()).c_str(), ERR_ERRONEUSNICKNAME(get_host(), clients[socket]->get_username()).size(), 0);
        else if (ft_check_nick(clients, command[1]))
            send(socket, ERR_NICKNAMEINUSE(get_host(), clients[socket]->get_username()).c_str(), ERR_NICKNAMEINUSE(get_host(), clients[socket]->get_username()).size(), 0);
        else
            client[socket]->set_nickname(command[1]);
    }
    if (client[socket]->get_pass() == "" || client[socket]->get_nickname() == "" || client[socket]->get_username() == "*")
        return 1;
    client[socket]->set_autho_status(true);
    std::cout << "\033[1;32m"
              << "User Connected "
              << "\033[0m" << std::endl;
    std::string message = RPL_WELCOME(get_host(), clients[socket]->get_nickname(), client[socket]->get_nickname() + "!" + client[socket]->get_username() + "@" + client[socket]->get_ip());
    send(socket, message.c_str(), message.size(), 0);
    return 0;
}

int Server::ft_get_buffer(std::vector<pollfd> &fd, std::map<int, User *> &clients, int i, std::vector<pollfd>::iterator pollitr)
{
    fcntl(fd[i].fd, F_SETFL, O_NONBLOCK);
    char buffer[512];
    bzero(buffer, 512);
    ssize_t bytesRead;
    bytesRead = recv(fd[i].fd, buffer, sizeof(buffer), 0);
    std::string ab = buffer;
    clients[fd[i].fd]->set_buffer(buffer);
    if (ab.find("\r\n") == std::string::npos)
        return 1;
    if (clients[fd[i].fd]->get_autho_status() == false)
    {
        ft_check_auten(clients, fd[i].fd);
        return 1;
    }
    fd[i].revents = 0;
    Commands(fd[i].fd, pollitr);
    return 0;
}

int Server::kick_out_client(int socket, std::map<int, User *> &clients, std::vector<pollfd>::iterator it)
{
    if (clients[socket]->get_autho_status())
        std::cout << "\033[1;31m"
                  << "User Disconected "
                  << "\033[0m" << std::endl;
    for (size_t j = 0; j < clients[socket]->get_chaine().size(); j++)
    {
        std::vector<Channels>::iterator itr = findChaine(clients[socket]->get_chaine()[j], this->Channel);
        if (itr != this->Channel.end())
        {
            int r = itr->getUsers().size();
            itr->getUsers().erase(itr->getUsers().find(socket));
            std::string form = FORMA(clients[socket]->get_username(), clients[socket]->get_nickname(), clients[socket]->get_ip());
            broadCast(*itr, PART(form, itr->getName(), "leaving ..."));
            if (std::find(itr->getOperators().begin(), itr->getOperators().end(), socket) != itr->getOperators().end())
            {
                if (itr->getOperators().size() == 1 && r > 1)
                {
                    itr->setOper(itr->getUsers().begin()->first);
                    std::string str = "MODE " + itr->getName() + " +o " + itr->getUsers().begin()->second + "\r\n";
                    std::cout << str;
                    broadCast(*itr, str);
                }
                itr->getOperators().erase(std::find(itr->getOperators().begin(), itr->getOperators().end(), socket));
            }
        }
    }
    close(socket);
    delete clients[socket];
    clients.erase(socket);
    fds.erase(it);
    return 0;
}

int Server::add_client(std::vector<pollfd> &fds, std::map<int, User *> &clients)
{
    pollfd tmp;
    fcntl(fd[0].fd, F_SETFL, O_NONBLOCK);
    sockaddr_in clinetadress;
    socklen_t clientAddrLen = sizeof(clinetadress);
    int clientSocket = accept(serversocket, (struct sockaddr *)&clinetadress, &clientAddrLen);
    if (clientSocket < 0)
    {
        std::cerr << "Error accepting connection" << std::endl;
        exit(EXIT_FAILURE);
    }
    tmp.fd = clientSocket;
    tmp.events = POLLIN;
    tmp.revents = 0;
    fds[0].revents = 0;
    fds.push_back(tmp);
    clients[clientSocket] = new User(clientSocket);
    clients[clientSocket]->set_add(clinetadress);
    return 0;
}

void Server::polling()
{
    pollfd tmp;
    tmp.fd = serversocket;
    tmp.events = POLLIN;
    tmp.revents = 0;
    fds.push_back(tmp);
    int pol = 0;
    this->fd = &fds[0];
    while (1)
    {
        pol = poll(fd, fds.size(), -1);
        if (pol == -1)
        {
            std::cerr << "Error polling connection" << std::endl;
            std::cerr << "Error poll" << std::endl;
            close(serversocket);
            std::map<int, User *> c = clients;
            std::vector<pollfd> fd = fds;
            close(fd[0].fd);
            for (size_t i = 1; i <= c.size(); i++)
            {
                delete c[fd[i].fd];
                close(fd[i].fd);
            }
            exit(EXIT_FAILURE);
        }
        if (fds[0].revents == POLLIN)
        {
            add_client(fds, clients);
            this->fd = &fds[0];
        }
        for (size_t i = 1; i < fds.size(); i++)
        {
            std::vector<pollfd>::iterator it;
            if (fd[i].revents == POLLIN)
            {
                it = fds.begin() + i;
                ft_get_buffer(fds, clients, i, it);
                break;
            }
            else if (fd[i].revents & POLLHUP)
            {
                it = fds.begin() + i;
                kick_out_client(fds[i].fd, clients, it);
                break;
            }
        }
    }
}

std::map<int, User *> &Server::get_clients()
{
    return (clients);
}

std::vector<pollfd>& Server::get_fds()
{
    return (fds);
}

Server::~Server()
{
    // for (std::map<int ,User *>::iterator i = clients.begin(); i != clients.end(); i++)
    // {
    //     close(i->first);
    //     delete i->second;
    // }
    // std::map<int, User *> c = clients;
    // std::vector<pollfd> fd = fds;
    // close(fd[0].fd);
    // for (size_t i = 1; i <= clients.size(); i++)
    // {
    //     delete clients[fd[i].fd];
    //     close(fd[i].fd);
    // }
}
