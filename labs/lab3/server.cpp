
#include <cstdio>
#include <iostream>
#include <winsock2.h>
#include <vector>
#include <string>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

struct book
{
    int reg_number = 0;
    std::string author;
    std::string title;
    int year = 0;
    std::string publisher;
    int pages = 0;
};

std::vector<book> books = {
    {1, "Pushkin", "Evgeniy Onegin", 1831, "Russian Classics", 400},
    {2, "Tolstoy", "War and Peace", 1869, "Russian Classics", 1225},
    {3, "Pushkin", "Belkin's Stories", 1831, "Russian Classics", 1000},
    {4, "Kolas", "Novaya Zyamlya", 1923, "Rodnaye Slova", 500},
    {5, "Dostoevsky", "Crime and Punishment", 1866, "The Russian Word", 671}
};

std::string view_books_by_author(const std::string& author_name)
{
    std::string response;
    for (const auto& book : books)
    {
        if (book.author == author_name)
        {
            response += "Reg. Number: " + std::to_string(book.reg_number) + "\n" +
                "Author: " + book.author + "\n" +
                "Title: " + book.title + "\n" +
                "Year: " + std::to_string(book.year) + "\n" +
                "Publisher: " + book.publisher + "\n" +
                "Pages: " + std::to_string(book.pages) + "\n\n";
        }
    }
    return response.empty() ? "No books found for the author: " + author_name : response;
}

void add_book(const std::string& data)
{
    std::stringstream ss(data);
    book new_book;
    char delimiter;
    ss >> new_book.reg_number >> delimiter;
    std::getline(ss, new_book.author, ';');
    std::getline(ss, new_book.title, ';');
    ss >> new_book.year >> delimiter;
    std::getline(ss, new_book.publisher, ';');
    ss >> new_book.pages;
    books.push_back(new_book);
}

void edit_book(const std::string& data)
{
    std::stringstream ss(data);
    int reg_number;
    char delimiter;
    ss >> reg_number >> delimiter;

    for (auto& book : books) 
    {
        if (book.reg_number == reg_number)
        {
            std::getline(ss, book.author, ';');
            std::getline(ss, book.title, ';');
            ss >> book.year >> delimiter;
            std::getline(ss, book.publisher, ';');
            ss >> book.pages;
            return;
        }
    }
}

void delete_book(int reg_number)
{
    books.erase(std::remove_if(books.begin(), books.end(), [reg_number](const book& b) { return b.reg_number == reg_number; }), books.end());
}

DWORD WINAPI client_handler(const LPVOID client_socket)
{
    const SOCKET client = *static_cast<SOCKET*>(client_socket);
    constexpr int buf_size = 1024;
    char buf[buf_size];
    int result;

    while ((result = recv(client, buf, sizeof(buf), 0)) > 0)
    {
        if (result <= buf_size - 1)
			buf[result] = '\0';
        else
        {
            std::clog << "Too many bytes received: may contain data loose!\n";
            buf[buf_size - 1] = '\0';
        }

        std::stringstream ss(buf);
        std::string command, data;
        ss >> command;
        std::getline(ss, data);
        data.erase(0, data.find_first_not_of(' '));

        std::string response;

        if (command == "VIEW")
            response = view_books_by_author(data);
        else if (command == "ADD") 
        {
            add_book(data);
            response = "Book added successfully.";
        }
        else if (command == "EDIT")
        {
            edit_book(data);
            response = "Book edited successfully.";
        }
        else if (command == "DELETE") 
        {
	        const int reg_number = std::stoi(data);
            delete_book(reg_number);
            response = "Book deleted successfully.";
        }
        else
            response = "Unknown command.";
        
        send(client, response.c_str(), static_cast<int>(response.size()), 0);
    }

    closesocket(client);
    return 0;
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

    const SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1280);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed.\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed.\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port 1280...\n";

    while (true)
    {
        SOCKET client_socket;
        sockaddr_in client_addr;
        int client_addr_size = sizeof(client_addr);

        client_socket = accept(server_socket, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_size);
        if (client_socket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed.\n" << WSAGetLastError();
            return 1;
        }

        DWORD thread_id;
        CreateThread(nullptr, 0, client_handler, &client_socket, 0, &thread_id);
    }
}
