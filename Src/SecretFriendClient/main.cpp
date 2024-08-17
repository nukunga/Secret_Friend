#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
// TODO: Openssl과 Crypto모듈 연동 테스트 후 지워도 됨
#include "..\ChatCrypto\ChatCrypto.h"

using namespace std;

// git push Test

#pragma comment (lib, "ws2_32.lib")

#define BUF_SIZE 1024
void    ErrorHandling(const char* message);

int main(int argc, char* argv[])
{
    // TODO: Openssl과 Crypto모듈 연동 테스트 후 지워도 됨
    TestCrypt();

    WSADATA        wsaData;
    SOCKET        hSocket;
    SOCKADDR_IN    servAdr;
    char        message[BUF_SIZE];
    int            strLen;
    WORD totalReadLen, readLen, dataSize;

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

    while (1)
    {
        cout << "Input message(Q to quit) : ";
        cin.getline(message, BUF_SIZE);

        if (!strcmp(message, "q") || !strcmp(message, "Q"))
            break;

        if (!strcmp(message, "w"))
        {
            strLen = strlen(message) + 1;

            BYTE packet[4096];
            memset(packet, 0x11, 4096);
            //memcpy(packet, "BOB", 4);
            send(hSocket, (const char*)packet, 4096, 0);
            printf("Send complete\n");
            //send(hSocket, "BOLSDSD", 8, 0);
        }
        else if (!strcmp(message, "e"))
        {
            strLen = strlen(message) + 1;

            BYTE packet[13];
            memset(packet, 0x00, 13);
            packet[0] = 0x24; packet[1] = 0x08; packet[2] = 0x01; packet[3] = 0x05;
            *((PWORD)(&packet[4])) = 7;

            for (int i = 6; i < 12; i++)
                packet[i] = 'A';

            send(hSocket, (const char*)packet, 7, 0);
            send(hSocket, (const char*)packet + 7, 6, 0);
            //send(hSocket, "BOLSDSD", 8, 0);
        }
        else
        {
            // 아래 코드는 패킷 분리현상을 검증하기 위해 일부러 나눠서 보냄
            strLen = strlen(message) + 1;

            BYTE packet[1024];
            //memcpy(packet, "BOB", 4);
            send(hSocket, "BOB", 4, 0);

            // IOCP와 달리 Overlapped 구조체를 거치지 않아 간편하게 코드 작성 가능
            totalReadLen = 0;
            while (totalReadLen < sizeof(WORD))
            {
                readLen = recv(hSocket, ((char*)&dataSize) + totalReadLen, sizeof(WORD) - totalReadLen, 0);
                if (readLen == 0)
                {
                    printf("Disconnected from server\n");
                    return 1;
                }

                totalReadLen += readLen;
            }

            totalReadLen = 0;
            readLen = 0;
            memset(packet, 0x00, sizeof(packet));
            printf("Received datasize: %d\n", dataSize);
            while (totalReadLen < dataSize)
            {
                readLen = recv(hSocket, (char*)&packet[totalReadLen], dataSize - readLen, 0);
                if (readLen == 0)
                {
                    printf("Disconnected from server\n");
                    return 1;
                }

                totalReadLen += readLen;
            }

            cout << "Message from server : " << packet << endl;
        }
        
    }
    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void    ErrorHandling(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}