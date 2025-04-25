
// ReSharper disable once CppInconsistentNaming
#define _WINSOCK_DEPRECATED_NO_WARNINGS  // NOLINT(bugprone-reserved-identifier, clang-diagnostic-reserved-macro-identifier)

#include <cstdio>
#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

void send_command(const SOCKET client_socket, const std::string& command, const std::string& data)
{
	const std::string message = command + " " + data;
    send(client_socket, message.c_str(), static_cast<int>(message.size()), 0);
}

int main()
{
    WSADATA wsa_data;
    constexpr WORD w_version_requested = MAKEWORD(2, 2);
    if (WSAStartup(w_version_requested, &wsa_data) != 0)
    {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    const SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1280);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
    {
        std::cerr << "Connect failed.\n";
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    while (true) 
    {
        std::cout << "Choose an operation: \n";
        std::cout << "1. View books by author\n";
        std::cout << "2. Add a new book\n";
        std::cout << "3. Edit a book\n";
        std::cout << "4. Delete a book\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        std::string command, data;
        switch (choice)
    	{
        case 1: 
        {
            std::cout << "Enter author name: ";
            std::string author_name;
            std::getline(std::cin, author_name);
            send_command(client_socket, "VIEW", author_name);
            break;
        }
        case 2: 
        {
            std::cout << "Enter new book details (format: RegNumber;Author;Title;Year;Publisher;Pages): ";
            std::getline(std::cin, data);
            send_command(client_socket, "ADD", data);
            break;
        }
        case 3: 
        {
            std::cout << "Enter the registration number of the book to edit: ";
            std::string reg_number;
            std::getline(std::cin, reg_number);
            std::cout << "Enter new book details (format: Author;Title;Year;Publisher;Pages): ";
            std::getline(std::cin, data);
            send_command(client_socket, "EDIT", reg_number + ";" += data);
            break;
        }
        case 4: 
        {
            std::cout << "Enter the registration number of the book to delete: ";
            std::string reg_number;
            std::getline(std::cin, reg_number);
            send_command(client_socket, "DELETE", reg_number);
            break;
        }
        case 5: 
        {
            std::cout << "Have a nice day!\n";
            closesocket(client_socket);
            WSACleanup();
            return 0;
        }
        default:
            std::cerr << "Invalid choice. Please try again.\n";
            continue;
        }

        constexpr int buf_size = 1024;
        char buf[buf_size];
        const int result = recv(client_socket, buf, sizeof(buf), 0);
        if (result > 0)
        {
            if (result <= buf_size - 1)
                buf[result] = '\0';
            else
            {
                std::clog << "Too many bytes received: may contain data loose!\n";
                buf[buf_size - 1] = '\0';
            }
            std::cout << "Response from server:\n" << buf << '\n';
        }
    }
}
