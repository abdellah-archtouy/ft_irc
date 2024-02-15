#include "Server.hpp"
#include "User.hpp"
Server server;

void handler(int sig)
{
    if (sig == SIGINT)
    {
        std::map<int, User *> c = server.get_clients();
        std::vector<pollfd> fd = server.get_fds();
        close(fd[0].fd);
        for (size_t i = 1; i <= c.size(); i++)
        {
            delete c[fd[i].fd];
            close(fd[i].fd);
        }
        exit(1);
    }
}

int main(int ac, char **av)
{
    if (ac == 3)
    {
        int i = 1;
        std::string line = av[i++];
        while (i < ac)
        {
            line = line + " ";
            line = line + av[i];
            i++;
        }
        signal(SIGINT, handler);
        signal(SIGPIPE, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        server.binding(line);
        std::cout << "Server listening on port " << av[1] << std::endl;
        server.polling();
    }
    else
    {
        std::cout << "argument error";
        exit(1);
    }
}