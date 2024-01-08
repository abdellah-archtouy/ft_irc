#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <limits>
#include <poll.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>


void handel(int s)
{
    std::cout <<s << " hii "<<  std::endl;
}

int main() {


    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        exit(EXIT_FAILURE);
    }


    // signal(SIGINT, handel);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(6767); 

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Error listening for connections" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port 6767..." << std::endl;
    int clientSocket = 0;
        // int clientSocket = accept(serverSocket, nullptr, nullptr);
    pollfd fds[OPEN_MAX];
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    int len = 1;
    int pol = 0;
    while (1)
    {
        std::cerr << "hbes hna "<< std::endl;
        pol = poll(fds, len + 1, -1);
        if (pol == -1){
            std::cerr << "Error polling connection" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (fds[0].revents && POLLIN)
        {
            std::cerr << "hani  hna "<< std::endl;
            clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == -1) {
                std::cerr << "Error accepting connection" << std::endl;
                exit(EXIT_FAILURE);
            }
            fds[len].fd = clientSocket;
            fds[len].events = POLLIN;
            fds[len].revents = 0;
            fds[0].revents = 0;
            len++;
        }
        for (size_t i = 1; i < len + 1; i++)
        {
            if (fds[i].revents && POLLIN)
            {
                char buffer[1024];
                ssize_t bytesRead;
                bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                buffer[bytesRead] = '\0';
                    std::cout << buffer <<  " " << bytesRead;
                    send(fds[i].fd, buffer, bytesRead, 0);
                fds[i].revents = 0;
                buffer[0] = '\0';
                break ;
            }
        }  
    }

        close(clientSocket);

    close(serverSocket);

    return 0;
}

        // char buffer[1024];
        // ssize_t bytesRead;

        // while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        //     // std::cout << buffer<< std::endl;
        //     send(clientSocket, buffer, bytesRead, 0);
        // }
            // if(fds[0].revents & POLLIN)
            // {
            //     std::cerr << "lalalla " << pol << std::endl;
            //     char buffer[1024];
            //     ssize_t bytesRead;

            //     while ((bytesRead = recv(fds[0].fd, buffer, sizeof(buffer), 0)) > 0) {
            //         // std::cout << buffer<< std::endl;
            //         send(fds[0].fd, buffer, bytesRead, 0);
            //     }
            //     continue;
            // }