#include "Bot.hpp"

int ft_check_param(char *p)
{
    std::string port = p;
    for (size_t i = 0; i < port.size(); i++)
    {
        if (!isdigit(port[i]))
            return 1;
    }
    return (0);
}

int main(int ac ,char **av)
{
    if(ac == 3)
    {
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            std::cerr << "Error creating socket" << std::endl;
            return 1;
        }
        if(ft_check_param(av[1]) || av[2][0] == '\0')
         {
            std::cout << "parameters Error" << std::endl;
            exit(1);
        }
        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
        serverAddress.sin_port = htons(atoi(av[1]));
        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Error connecting to server" << std::endl;
            close(clientSocket);
            return 1;
        }
        std::string pass = av[2];
        std::string word =  "PASS "+ pass +"\r\n";
        send(clientSocket, word.c_str(), word.size(), 0);
        word =  "NICK bot\r\n";
        usleep (200);
        send(clientSocket, word.c_str(), word.size(), 0);
        word =  "USER bot 0 * bot\r\n";
        usleep (200);
        send(clientSocket, word.c_str(), word.size(), 0);
        char buffer[1024];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        std::string h;
        std::string word1;
        std::vector<std::string> param;
        std::stringstream l;
        while (1)
        {
            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            h = buffer;
            l.str(h);
            h.clear();
            std::getline(l, h, '\r');
            l.clear();
            l.str(h);
            std::string tmp;
            while (std::getline(l, word1, ' '))
                param.push_back(word1);
            if(param.size() != 1)
                tmp = "";
            else
                tmp = "PRIVMSG " + param[0] + " :You are so annoying\r\n";
            send(clientSocket, tmp.c_str(), tmp.size(),0);
            if(bytesRead == 0)
                break;
            l.clear();
            h.clear();
            param.clear();
        }
        close(clientSocket);
    }
    else
        std::cerr << "Error Parameters" <<std::endl;
    return 0;
}
