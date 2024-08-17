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


    //client가 사용할 대칭키(AES) 생성 및 저장
    //공개키와  개인키도 생성하여 저장(AES)
    //생성하고 대칭키를 서버에 전송 
    //패킷 데이터로 OK를 리턴 받으면 continue 아니면 프로그램 종료
    //OK이후 Client의 공개키를 서버에 전송
    //이후 서버가 룸리스트를 보내주면 \0(NULL값)을 기준으로 파싱하여
    Network session(hSocket);

    session.CreatePrivateKey();
    session.CreatePublicKey();
    session.CreateSymmetricKey();
    session.CreateIV();

    //session.PrintPrivateKey();
    //session.PrintPublicKey();
    
    session.SendSymmetricKey();
    session.SendPublicKey();


    //화면에 출력
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
            size_t spacePos = menuChoice.find(' ');  // 공백 위치 찾기

            if (spacePos != string::npos && spacePos + 1 < menuChoice.size())
            {
                string roomNumberStr = menuChoice.substr(spacePos + 1);  // 공백 뒤의 문자열을 추출
                int roomNumber = stoi(roomNumberStr);  // 문자열을 정수로 변환

                //// 방 번호 유효성 확인
                if (roomNumber >= 0 && roomNumber < session.RoomList.size())
                {
                    std::vector<BYTE> roomName = { session.RoomList[roomNumber] };
                    session.SendPacket(roomName, CLIENT_REQ_ROOM_ENTER);
                }
            }
            break;
        }


        case 'I':
            //서버에 방 목록 요청(110) 패킷 전송
            //서버에서 응답 받으면 데이터를 '\0' NULL값을 기준으로 파싱하여 화면에 출력
            session.SendPacket({'\0'}, CLIENT_REQ_ROOM_LIST);
            
            break;

        case 'C':
        {
            // menuChoice에서 스페이스바를 구분하여 데이터(room name)을 파싱하여 서버에 전송
            // room 대칭키 생성, 채팅창 화면 출력, 입력 아무것도 안받으면서 대기
            // 서버로부터 공개키값(212) 전달 받으면 공개키로 room의 대칭키를 서버에 전송
            // 서버로부터 Sucess값 받으면 채팅창 화면에 상대방이 입장하였습니다. 출력하면서
            // 1:1 익명 채팅창 시작 -> 채팅 창 화면으로 넘어가면서 상대방 입력 받는 스레드와 내가 입력하는스레드 
            // 생성 하여 내가 만든 대칭키를 전송
            size_t spacePos = menuChoice.find(' ');  // 공백 위치 찾기

            if (spacePos != string::npos && spacePos + 1 < menuChoice.size())
            {
                string roomNameStr = menuChoice.substr(spacePos + 1);  // 공백 뒤의 문자열을 추출

                //// 방 번호 유효성 확인
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
