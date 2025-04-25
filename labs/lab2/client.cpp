
// ReSharper disable once CppInconsistentNaming
#define _WINSOCK_DEPRECATED_NO_WARNINGS  // NOLINT(clang-diagnostic-reserved-macro-identifier, bugprone-reserved-identifier)

#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) 
    {
        std::cerr << "Failed to initialize Winsock: " << WSAGetLastError() << '\n';
        return 1;
    }

    const SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket == INVALID_SOCKET) 
    {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    constexpr int buffer_size = 1024;
    char buffer[buffer_size];
    std::string input;

    std::cout << "Enter a string to send to the server: ";
    std::getline(std::cin, input);

    if (sendto(client_socket, input.c_str(), static_cast<int>(input.length()), 0, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) 
    {
        std::cerr << "Failed to send data: " << WSAGetLastError() << '\n';
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    int server_addr_size = sizeof(server_addr);
    const int bytes_received = recvfrom(client_socket, buffer, buffer_size, 0, reinterpret_cast<sockaddr*>(&server_addr), &server_addr_size);
    if (bytes_received == SOCKET_ERROR)
        std::cerr << "Failed to receive data: " << WSAGetLastError() << '\n';
    else if (bytes_received <= buffer_size - 1)
        buffer[bytes_received] = '\0';
    else
    {
        std::clog << "Too many bytes received: may contain data loose!\n";
        buffer[buffer_size - 1] = '\0';
    }

	std::cout << "Response from server: " << buffer << '\n';

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
