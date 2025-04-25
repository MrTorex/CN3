
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

struct Product {
    std::string country;
    std::string firm;
    std::string name;
    int quantity;
};

void reaper(int sig) 
{
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

void handleClient(int clientSocket, Product products[], int productCount) 
{
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::string requestedCountry = buffer;
    std::string response;

    for (int i = 0; i < productCount; ++i) 
    {
        if (products[i].country == requestedCountry) 
        {
            response += "Фирма: " + products[i].firm + ", ";
            response += "Наименование: " + products[i].name + ", ";
            response += "Количество: " + std::to_string(products[i].quantity) + "\n";
        }
    }

    send(clientSocket, response.c_str(), response.size(), 0);
    close(clientSocket);
}

int main() 
{
    setlocale(LC_ALL, "Russian");
    const int PORT = 7500;
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr{};
    struct sockaddr_in clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);

    Product products[5] = {
        {"USA", "Blueberry", "Blueberry Phone", 50},
        {"China", "Huawei", "P40", 30},
        {"Korea", "Samsung", "Galaxy S21", 40},
        {"Germany", "Bosch", "Drill", 25},
        {"Japan", "Sony", "PlayStation 5", 20}
    };

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) 
    {
        std::cerr << "Ошибка создания сокета!" << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Ошибка привязки сокета!" << std::endl;
        return 1;
    }

    listen(serverSocket, 5);
    signal(SIGCHLD, reaper);

    std::cout << "Сервер запущен и ожидает подключения клиентов..." << std::endl;

    while (true) 
    {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket < 0) 
        {
            std::cerr << "Ошибка принятия соединения!" << std::endl;
            continue;
        }

        if (fork() == 0)
        {
            close(serverSocket);
            handleClient(clientSocket, products, 5);
            exit(0);
        } else {
            close(clientSocket);
        }
    }

    close(serverSocket);
    return 0;
}
