#include "Server.hpp"


Server::Server()
{
    serversocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serversocket == -1){
        std::cerr << "Error creating socket" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::binding(char **av)
{

    _pass = av[2];
    _port = av[1];
    sockaddr_in ServerAddress;
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    ServerAddress.sin_port = htons(atoi(_port.c_str()));

    if (bind(serversocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(serversocket, 10) == -1) {
        std::cerr << "Error listening for connections" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int Server::ft_check_auten(std::map<int, User *>client, int socket)
{
    std::string buffer = client[socket]->get_buffer().substr(0, client[socket]->get_buffer().find("\r\n"));
    std::stringstream split(buffer);
    std::string word;
    std::vector<std::string> command;
    std::getline(split, word,' ');
    command.push_back(word);
    std::getline(split, word);
    command.push_back(word);
    if (command.size() < 2)
        return std::cerr << "you should insert second argument" << std::endl, 1;
    if (client[socket]->get_pass() == "")
    {
        if (command[0] == "PASS")
        {
            if (command[1] == this->_pass)
                client[socket]->set_pass(command[1]);
            else
                return std::cerr << "password incorect" << std::endl,1;
            return 1;
        }
        else
            return std::cerr << "you need first to authenticate" << std::endl,1;
    }
    else if (client[socket]->get_nikename() == "")
    {
        if (command[0] == "NICK")
        {
            client[socket]->set_nikename(command[1]);
            return 1;
        }
        else
            return std::cerr << "you need first to authenticate" << std::endl,1;
    }
    else if (client[socket]->get_username() == "")
    {
        if (command[0] == "USER")
        {
            client[socket]->set_username(command[1]);
        }
        else
            return std::cerr << "you need first to authenticate" << std::endl,1;
    }
    client[socket]->set_autho_status(true);
    std::cout << "\033[1;32m" << "User Connected " << "\033[0m" << std::endl;
    send(socket, ":e1r3p7.1337.ma 001 user :Welcome to the Internet Relay Network user!user@10.11.3.7\r\n", 512, 0);
    return 0;
}


void Server::polling()
{
    int clientSocket = 0;
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
        if (pol == -1){
            std::cerr << "Error polling connection" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (fds[0].revents & POLLIN)
        {
            fcntl(fd[0].fd, F_SETFL, O_NONBLOCK);
            sockaddr_in clinetadress;
            socklen_t clientAddrLen = sizeof(clinetadress);
            clientSocket = accept(serversocket, (struct sockaddr*)&clinetadress, &clientAddrLen);
            if (clientSocket < 0) {
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
        }
        this->fd = &fds[0];
        for (size_t i = 1; i < fds.size(); i++)
        {
            std::vector<pollfd>::iterator it;
            if (fd[i].revents == POLLIN)
            {
                fcntl(fd[i].fd, F_SETFL, O_NONBLOCK);
                char buffer[512];
                bzero(buffer, 512);
                it = fds.begin() + i;
                ssize_t bytesRead;
                bytesRead = recv(fd[i].fd, buffer, sizeof(buffer), 0);
                std::string ab = buffer;
                clients[fds[i].fd]->set_buffer(buffer);
                if (ab.find("\r\n") == std::string::npos)
                    break ;
                if (clients[fds[i].fd]->get_autho_status() == false)
                {
                    ft_check_auten(clients, fds[i].fd);
                    break;
                }
                std::cout << clients[fds[i].fd]->get_buffer() << std::endl;
                // send(fd[i].fd, buffer, bytesRead, 0);
                fd[i].revents = 0;
                break ;
            }
            else if (fd[i].revents & POLLHUP)
            {
                it = fds.begin() + i;
                close(fds[i].fd);
                std::cout << "\033[1;31m" << "User Desconected " << "\033[0m" << std::endl;
                delete clients[fds[i].fd];
                clients.erase(fds[i].fd);
                fds.erase(it);
                break;
            }
        }
    }
}

std::map<int, User *> Server::get_clients()
{
    return (clients);
}

std::vector<pollfd> Server::get_fds()
{
    return (fds);
}

Server::~Server()
{
}

                // if (!clients[fds[i].fd]->get_autho_status())
                // {
                //     std::stringstream a(clients[fds[i].fd]->get_buffer());
                //     std::string first;
                //     std::string second;
                //     std::getline(a,first,' ');
                //     std::getline(a,second);
                //     if (first != "PASS" && clients[fds[i].fd]->get_pass() == "")
                //     {
                //         std::cout << "You shuold to authenticate" << std::endl;
                //         send(fds[i].fd, "You shuold to authenticate", 27, 0);
                //         break;
                //     }
                //     else if (first == "PASS")
                //     {
                //         if (clients[fds[i].fd]->get_pass() != "")
                //         {
                //             std::cout << "password is already set" << std::endl;
                //             send(fds[i].fd, "password is already set", 24, 0);
                //             break;
                //         }
                //         else if (second == this->_pass)
                //         {
                //             clients[fds[i].fd]->set_pass(second);
                //             std::cout << "password seted successfully" << std::endl;
                //             send(fds[i].fd, "password seted successfully", 28, 0);
                //             break;
                //         }
                //     }
                //     else if (first != "NICK" && clients[fds[i].fd]->get_nikename() == "")
                //     {
                //         std::cout << "You shuold to authenticate" << std::endl;
                //         send(fds[i].fd, "You shuold to authenticate", 27, 0);
                //         break;
                //     }
                //     else if (first == "NICK")
                //     {
                //         if (clients[fds[i].fd]->get_nikename() != "")
                //         {
                //             std::cout << "NICK NAME is already set" << std::endl;
                //             send(fds[i].fd, "NICK NAME is already set", 25, 0);
                //             break;
                //         }
                //         else
                //         {
                //             clients[fds[i].fd]->set_nikename(second);
                //             std::cout << "NICK NAME seted successfully" << std::endl;
                //             send(fds[i].fd, "NICK NAME seted successfully", 29, 0);
                //             break;
                //         }
                //     }
                //     else if (first != "USER" && clients[fds[i].fd]->get_username() == "")
                //     {
                //         std::cout << "You shuold to authenticate" << std::endl;
                //         send(fds[i].fd, "You shuold to authenticate", 27, 0);
                //         break;
                //     }
                //     else if (first == "USER")
                //     {
                //         if (clients[fds[i].fd]->get_username() != "")
                //         {
                //             std::cout << "NAME is already set" << std::endl;
                //             send(fds[i].fd, "NAME is already set", 24, 0);
                //             break;
                //         }
                //         else
                //         {
                //             clients[fds[i].fd]->set_username(second);
                //             std::cout << "NAME seted successfully" << std::endl;
                //             send(fds[i].fd, "NAME seted successfully", 28, 0);
                //         }
                //     }
                //     else
                //     {
                //         std::cout << "You shuold to authenticate" << std::endl;
                //         send(fds[i].fd, "You shuold to authenticate", 28, 0);
                //         break;
                //     }
                //     clients[fds[i].fd]->set_autho_status(true);
                //     std::cout << "\033[1;32m" << "User Connected " << "\033[0m" << std::endl;
                // }