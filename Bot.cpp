#include "Bot.hpp"

std::string save_nick(std::string m)
{
    return(m.substr(1, m.find('!') - 1));
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("10.11.3.7");
    serverAddress.sin_port = htons(8080);
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to server" << std::endl;
        close(clientSocket);
        return 1;
    }
    std::string word =  "PASS 123\r\n";
    send(clientSocket, word.c_str(), word.size(), 0);
    word =  "NICK boot\r\n";
    usleep (100);
    send(clientSocket, word.c_str(), word.size(), 0);
    word =  "USER boot 0 * boooot\r\n";
    usleep (100);
    send(clientSocket, word.c_str(), word.size(), 0);
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    std::string h;
    std::string nick;
    std::stringstream l;
    while (1)
    {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        h = buffer;
        nick = save_nick(h);
        l.str(h);
        h.clear();
        std::getline(l, h, '\r');
        l.str(h);
        while (l >> h);

        if (h == "pile" || h == "face")
        {
            std::string tmp;
            std::string tmp1;
            int a = std::rand();
            if (a % 2)
            {
                tmp.clear();
                if (h == "face")
                    tmp1 = "you lose";
                else
                    tmp1 = "you win";
                tmp = "PRIVMSG " + nick + " : pile \r\n";
                send(clientSocket, tmp.c_str(), tmp.size(),0);
                tmp = "PRIVMSG " + nick + " : " + tmp1 +"\r\n";
                usleep(100);
                send(clientSocket, tmp.c_str(), tmp.size(),0);
            }
            else
            {
                if (h != "face")
                    tmp1 = "you lose";
                else
                    tmp1 = "you win";
                tmp = "PRIVMSG " + nick + " : face \r\n";
                send(clientSocket, tmp.c_str(), tmp.size(),0);
                usleep(100);
                tmp = "PRIVMSG " + nick + " : " + tmp1 +"\r\n";
                send(clientSocket, tmp.c_str(), tmp.size(),0);
            }
        }
        else 
        {
            std::string tmp = "PRIVMSG " + nick + " : you need to send pile or face\r\n";
            send(clientSocket, tmp.c_str(), tmp.size(),0);
        }
        if(bytesRead == 0)
            break;
        l.clear();
        h.clear();
    }
    close(clientSocket);
    return 0;
}
