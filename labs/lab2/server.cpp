
#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

std::string process_string(const std::string& input)
{
	const int length = static_cast<int>(input.length());

    if (length % 4 == 0 && length != 0) 
    {
	    const std::string part1 = input.substr(0, length / 2);
	    const std::string part2 = input.substr(length / 2);
        return part2 + part1;
    }
    return input;
}

int main()
{
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) 
    {
        std::cerr << "Failed to initialize Winsock: " << WSAGetLastError() << '\n';
        return 1;
    }

    const SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket == INVALID_SOCKET) 
    {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) 
    {
        std::cerr << "Socket bind failed: " << WSAGetLastError() << '\n';
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    constexpr int buffer_size = 1024;
    char buffer[buffer_size];
    sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);

    std::cout << "Server is ready to receive messages..." << '\n';

    const int bytes_received = recvfrom(server_socket, buffer, buffer_size, 0, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_size);
    if (bytes_received == SOCKET_ERROR) 
    {
        std::cerr << "Failed to receive data: " << WSAGetLastError() << '\n';
        return 1;
    }

    if (bytes_received <= buffer_size - 1)
        buffer[bytes_received] = '\0';
    else
    {
        std::clog << "Too many bytes received: may contain data loose!\n";
        buffer[buffer_size - 1] = '\0';
    }

    const std::string received_string(buffer);
    std::cout << "Message received from client: " << received_string << '\n';

    const std::string processed_string = process_string(received_string);

    if (sendto(server_socket, processed_string.c_str(), static_cast<int>(processed_string.length()), 0, reinterpret_cast<sockaddr*>(&client_addr), client_addr_size) == SOCKET_ERROR)
        std::cerr << "Failed to send data: " << WSAGetLastError() << '\n';

    closesocket(server_socket);
    WSACleanup();
    return 0;
}