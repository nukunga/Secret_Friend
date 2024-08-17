#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <winsock2.h>
#include <thread>
#include <atomic>
#include "Packet.h"
#include "Network.h"
#include "Key.h"

using namespace std;

#pragma comment (lib, "ws2_32.lib")

#define BUF_SIZE 1024
void ErrorHandling(const char* message);
void showMenu();
//void clearScreen();
void sendMessage(SOCKET hSocket, std::atomic<bool>& isRunning);
void receiveMessage(SOCKET hSocket, std::atomic<bool>& isRunning);

int main(int argc, char* argv[])
{

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
    servAdr.sin_addr.s_addr = inet_addr("192.168.3.226");
    servAdr.sin_port = htons(8000);

    if (::connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("connect() error");
    else
        cout << "Connected.................\n";


    //client�� ����� ��ĪŰ(AES) ���� �� ����
    //����Ű��  ����Ű�� �����Ͽ� ����(AES)
    //�����ϰ� ��ĪŰ�� ������ ���� 
    //��Ŷ �����ͷ� OK�� ���� ������ continue �ƴϸ� ���α׷� ����
    //OK���� Client�� ����Ű�� ������ ����
    //���� ������ �븮��Ʈ�� �����ָ� \0(NULL��)�� �������� �Ľ��Ͽ�
    Network session(hSocket);

    session.CreatePrivateKey();
    session.CreatePublicKey();
    session.CreateSymmetricKey();
    session.CreateIV();

    //session.PrintPrivateKey();
    //session.PrintPublicKey();
    
    session.SendSymmetricKey();
    session.SendPublicKey();


    //ȭ�鿡 ���
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
        {
            size_t spacePos = menuChoice.find(' ');  // ���� ��ġ ã��

            if (spacePos != string::npos && spacePos + 1 < menuChoice.size())
            {
                string roomNumberStr = menuChoice.substr(spacePos + 1);  // ���� ���� ���ڿ��� ����
                int roomNumber = stoi(roomNumberStr);  // ���ڿ��� ������ ��ȯ

                //// �� ��ȣ ��ȿ�� Ȯ��
                if (roomNumber >= 0 && roomNumber < session.RoomList.size())
                {
                    std::vector<BYTE> roomName = { session.RoomList[roomNumber] };
                    session.SendPacket(roomName, CLIENT_REQ_ROOM_ENTER);
                }
            }
            break;
        }


        case 'I':
            //������ �� ��� ��û(110) ��Ŷ ����
            //�������� ���� ������ �����͸� '\0' NULL���� �������� �Ľ��Ͽ� ȭ�鿡 ���
            session.SendPacket({'\0'}, CLIENT_REQ_ROOM_LIST);
            
            break;

        case 'C':
        {
            // menuChoice���� �����̽��ٸ� �����Ͽ� ������(room name)�� �Ľ��Ͽ� ������ ����
            // room ��ĪŰ ����, ä��â ȭ�� ���, �Է� �ƹ��͵� �ȹ����鼭 ���
            // �����κ��� ����Ű��(212) ���� ������ ����Ű�� room�� ��ĪŰ�� ������ ����
            // �����κ��� Sucess�� ������ ä��â ȭ�鿡 ������ �����Ͽ����ϴ�. ����ϸ鼭
            // 1:1 �͸� ä��â ���� -> ä�� â ȭ������ �Ѿ�鼭 ���� �Է� �޴� ������� ���� �Է��ϴ½����� 
            // ���� �Ͽ� ���� ���� ��ĪŰ�� ����
            size_t spacePos = menuChoice.find(' ');  // ���� ��ġ ã��

            if (spacePos != string::npos && spacePos + 1 < menuChoice.size())
            {
                string roomNameStr = menuChoice.substr(spacePos + 1);  // ���� ���� ���ڿ��� ����

                //// �� ��ȣ ��ȿ�� Ȯ��
                if (roomNameStr.size() > 0)
                {
                    std::vector<BYTE> roomName(roomNameStr.begin(), roomNameStr.end());
                    session.SendPacket(roomName, CLIENT_REQ_ROOM_ENTER);
                }
            }
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
    cout << "********** Menu **********\n";
    cout << "J : Enter the Room\n";
    cout << "I : Reloading the Room\n";
    cout << "C : Create the Room\n";
    cout << "R : Enter the Random Room\n";
    cout << "Q : Quit\n";
    cout << "*********Room list*********\n";
    //for()
}

//void clearScreen()
//{
//#ifdef _WIN32
//    system("cls");
//#else
//    system("clear");
//#endif
//}
