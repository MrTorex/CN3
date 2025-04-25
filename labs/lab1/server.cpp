
#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int gcd(int a, int b) 
{
    while (b != 0) 
    {
	    const int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int main()
{
    WSADATA wsa_data;
    sockaddr_in server_addr, client_addr;

    constexpr int port = 54000;
    int client_addr_size = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) 
    {
        std::cout << "Error while initialasing Winsock!\n";
        return 1;
    }

    const SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) 
    {
        std::cout << "Error while socket creation!\n";
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) 
    {
        std::cout << "Error while socket connecting!\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 1) == SOCKET_ERROR) 
    {
        std::cout << "Error while listening port!\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for client connection...\n";
    const SOCKET client_socket = accept(server_socket, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_size);
    if (client_socket == INVALID_SOCKET) 
    {
        std::cout << "Error while client connecting!\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client is connected.";

    while (true) 
    {
        int num1, num2;
        int recv_result = recv(client_socket, reinterpret_cast<char*>(&num1), sizeof(num1), 0);
        if (recv_result <= 0) break;

        recv_result = recv(client_socket, reinterpret_cast<char*>(&num2), sizeof(num2), 0);
        if (recv_result <= 0) break;

        num1 = static_cast<int>(ntohl(num1));
        num2 = static_cast<int>(ntohl(num2));

        int result = gcd(num1, num2);
        result = static_cast<int>(htonl(result));

        send(client_socket, reinterpret_cast<char*>(&result), sizeof(result), 0);
    }

    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
