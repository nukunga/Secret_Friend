#include <winsock2.h>
#include <iostream>
#include <process.h>
#include <string>
#include <windows.h>
#include "IOCP.h"

// TODO: Openssl�� Crypto��� ���� �׽�Ʈ �� ������ ��
#include "..\ChatCrypto\ChatCrypto.h"

int main(int argc, char* argv[])
{   
    // TODO: Openssl�� Crypto��� ���� �׽�Ʈ �� ������ ��
    TestCrypt();

    constexpr int port = 8000;
    std::unique_ptr<IOCP> server = std::make_unique<IOCP>();

    if (server->InitializeSocket() == false)
    {
        return 0;
    }

    if (server->InitializeServer(port) == false)
    {
        return 0;
    }

    printf("[+] quit�� �Է½� �����մϴ�.\n");
    while (true)
    {
        std::string inputCmd;
        std::getline(std::cin, inputCmd);

        if (inputCmd == "quit")
        {
            break;
        }
    }

    server->StopServer();

    printf("[-] Server closed. Press any key to exit.\n");
    getchar();

    return 0;
}