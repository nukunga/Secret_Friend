#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <winsock2.h>
#include <thread>
#include <atomic>
// TODO: Openssl과 Crypto모듈 연동 테스트 후 지워도 됨
#include "..\ChatCrypto\ChatCrypto.h"

using namespace std;

#pragma comment (lib, "ws2_32.lib")

#define BUF_SIZE 1024
void ErrorHandling(const char* message);
void showMenu();
void clearScreen();
void sendMessage(SOCKET hSocket, std::atomic<bool>& isRunning);
void receiveMessage(SOCKET hSocket, std::atomic<bool>& isRunning);

int main(int argc, char* argv[])
{
    // TODO: Openssl과 Crypto모듈 연동 테스트 후 지워도 됨
    TestCrypt();

    WSADATA        wsaData;
    SOCKET        hSocket;
    SOCKADDR_IN    servAdr;
    std::atomic<bool> isRunning(true);

    if (argc != 3)
    {
        cout << "Usage : " << argv[0] << " <IP> <port>\n";
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling("socket() error");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAdr.sin_port = htons(8000);

    if (::connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("connect() error");
    else
        cout << "Connected.................\n";

    showMenu();

    while (1)
    {
        cout << "Select menu (J/I/C/R/Q): ";
        string menuChoice;
        getline(cin, menuChoice);

        if (menuChoice.empty())
        {
            cout << "Invalid choice! Please select again.\n";
            continue;
        }

        char choice = toupper(menuChoice[0]);

        switch (choice)
        {
        case 'Q':
            isRunning = false;
            break;

        case 'J':
        case 'I':
        case 'C':
        case 'R':
        {
            wstring roomName;
            if (choice == 'J' || choice == 'C')
            {
                wcout << L"Enter room name: ";
                getline(wcin, roomName);
                // Send room name to server and handle room entry logic
            }

            std::thread sendThread(sendMessage, hSocket, std::ref(isRunning));
            std::thread receiveThread(receiveMessage, hSocket, std::ref(isRunning));

            sendThread.join();
            receiveThread.join();

            clearScreen();
            showMenu();
            break;
        }

        default:
            cout << "Invalid choice! Please select again.\n";
            break;
        }

        if (choice == 'Q')
            break;
    }

    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void sendMessage(SOCKET hSocket, std::atomic<bool>& isRunning)
{
    while (isRunning)
    {
        wstring chatMessage;
        wcout << L"Enter message (!q to quit): ";
        getline(wcin, chatMessage);

        if (chatMessage == L"!q")
        {
            isRunning = false;
            break;
        }

        // Convert wstring to string (assuming UTF-8)
        string message(chatMessage.begin(), chatMessage.end());
        send(hSocket, message.c_str(), message.size() + 1, 0); // +1 to send the null terminator
    }
}

void receiveMessage(SOCKET hSocket, std::atomic<bool>& isRunning)
{
    char message[BUF_SIZE];
    int strLen;

    while (isRunning)
    {
        memset(message, 0, BUF_SIZE);
        strLen = recv(hSocket, message, BUF_SIZE - 1, 0);
        if (strLen == 0 || strLen == SOCKET_ERROR)
        {
            isRunning = false;
            cout << "Disconnected from server\n";
            break;
        }

        cout << "Message from server: " << message << endl;
    }
}

void ErrorHandling(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void showMenu()
{
    cout << "Menu:\n";
    cout << "J : Enter the Room\n";
    cout << "I : Reloading the Room\n";
    cout << "C : Create the Room\n";
    cout << "R : Enter the Random Room\n";
    cout << "Q : Quit\n";
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
