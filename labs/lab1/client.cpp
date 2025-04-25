
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsa_data;
    sockaddr_in server_addr;

    const auto server_ip = "127.0.0.1";
    constexpr int port = 54000;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) 
    {
        std::cout << "Error while Winsock initialasing!\n";
        return 1;
    }

    const SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) 
    {
        std::cout << "Error while socket creation!\n";
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(port);

    if (connect(client_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) 
    {
        std::cout << "Error while connection to the server\n";
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server.\n";

    int num1, num2;
    std::cout << "Enter two numbers: ";
    std::cin >> num1 >> num2;

    num1 = static_cast<int>(htonl(num1));
    num2 = static_cast<int>(htonl(num2));

    send(client_socket, reinterpret_cast<char*>(&num1), sizeof(num1), 0);
    send(client_socket, reinterpret_cast<char*>(&num2), sizeof(num2), 0);

    int result;
    const int recv_result = recv(client_socket, reinterpret_cast<char*>(&result), sizeof(result), 0);

    if (recv_result > 0) 
    {
        result = static_cast<int>(ntohl(result));
        std::cout << "GCD: " << result << '\n';
    }
    else
        std::cout << "Error while receiving data from server!\n";

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
