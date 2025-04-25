
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() 
{
    setlocale(LC_ALL, "Russian");
    const int PORT = 7500;
    const char *SERVER_IP = "127.0.0.1";
    int clientSocket;
    struct sockaddr_in serverAddr{};
    char buffer[256];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) 
    {
        std::cerr << "Ошибка создания сокета!" << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) 
    {
        std::cerr << "Ошибка подключения к серверу!" << std::endl;
        return 1;
    }

    std::cout << "Введите страну-изготовителя для поиска товаров: ";
    std::string country;
    std::getline(std::cin, country);

    send(clientSocket, country.c_str(), country.size(), 0);

    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Товары из указанной страны:\n" << buffer << std::endl;

    close(clientSocket);
    return 0;
}
