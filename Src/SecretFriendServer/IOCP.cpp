#include "IOCP.h"
#include <process.h>

bool IOCP::InitializeSocket()
{
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[-] WSA startup fail\n");
        exit(0);
    }

    ServerSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

    if (ServerSock == INVALID_SOCKET)
    {
        printf("[-] WSAsocket() fail: %d\n", WSAGetLastError());
        return false;
    }

    printf("[+] Socket initialized\n");
    return true;
}

bool IOCP::InitializeServer(int port)
{
    SOCKADDR_IN serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(ServerSock, (SOCKADDR*)&serverAddress, sizeof(SOCKADDR_IN)) != 0)
    {
        printf("[-] bind() failed: %d\n", WSAGetLastError());
        return false;
    }

    // 접속 Queue 5개
    if (listen(ServerSock, 5) != 0)
    {
        printf("[-] listen() failed: %d\n", WSAGetLastError());
        return false;
    }

    printf("[+] Server is now listening on port %d\n", port);

    IocpHandle = CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,
        NULL,
        NULL,
        maximumThread
    );

    if (IocpHandle == NULL)
    {
        printf("[-] CreateIoCompletionPort() failed: %d\n", GetLastError());
        return false;
    }

    if (CreateThreads() == false)
        return false;

    printf("[+] IOCP successfully initialized\n");
    return true;
}

bool IOCP::CreateThreads()
{
    acceptThread = std::thread(&IOCP::AcceptThread, this);
    printf("[+] Accept thread is now running\n");

    for (DWORD i = 0; i < sysInfo.dwNumberOfProcessors; i++)
    {
        workerThreads.emplace_back(std::thread(&IOCP::WorkerThread, this));
    }
    printf("[+] IOCP Worker thread is now running (%d threads)\n", maximumThread);
    return true;
}


bool IOCP::StopServer()
{
    workerRunning = false;

    for (size_t i = 0; IocpHandle != NULL && i < workerThreads.size(); ++i)
    {
        PostQueuedCompletionStatus(IocpHandle, 0, NULL, NULL);
    }

    for (size_t i = 0; i < workerThreads.size(); ++i)
    {
        workerThreads[i].join();
    }

    acceptRuning = false;
    closesocket(ServerSock);
    if (IocpHandle != NULL)
    {
        CloseHandle(IocpHandle);
    }

    acceptThread.join();

    return true;
}

unsigned int IOCP::AcceptThread()
{
    SOCKADDR_IN clientAddress;
    int nAddrLen = sizeof(SOCKADDR_IN);
    while (acceptRuning)
    {
        SOCKET newSocket = accept(ServerSock, (SOCKADDR*)&clientAddress, &nAddrLen);

        if (newSocket == INVALID_SOCKET)
            continue;
        
        Session* session = new Session(newSocket);
        if (session == NULL)
            continue;

        session->Connect(IocpHandle);
        AddNewSession(session);
    }
    printf("[-] Stop AcceptThread\n");
    return 0;
}

void IOCP::AddNewSession(Session* session)
{
    this->connectedClients.push_back(session);
}

void IOCP::DeleteSession(Session* session)
{
    session->Close();
    this->connectedClients.remove(session);
    delete session;
}

unsigned IOCP::WorkerThread()
{
    unsigned uResult = 0;
    DWORD dwIoSize = 0;
    BOOL bSuccess = TRUE;
    Session* pSession = nullptr;
    LPOVERLAPPED lpOverlapped = nullptr;

    while (workerRunning)
    {
        try
        {
            bSuccess = GetQueuedCompletionStatus(IocpHandle, &dwIoSize, (PULONG_PTR)&pSession, &lpOverlapped, INFINITE);

            if (bSuccess == TRUE && dwIoSize == 0 && lpOverlapped == nullptr)
            {
                workerRunning = false;
                continue;
            }

            if (lpOverlapped == nullptr) continue;

            if (bSuccess == FALSE || (dwIoSize == 0 && bSuccess == TRUE))
            {
                DeleteSession(pSession);
                continue;
            }

            IO_DATA* ioData = (IO_DATA*)lpOverlapped;
            if (IO_SEND == ioData->Type())
            {
                printf("[SEND] Send packet to %d\n", pSession);
                //delete ioData;
            }
            else if (IO_RECV == ioData->Type())
            {
                printf("[RECV] Received packet from %d (size: %d Bytes)\n", pSession, dwIoSize);
                // 패킷 처리
                pSession->PushRecvPacket(*ioData, dwIoSize);
                pSession->ValidatePacket();
                
                // TODO: 암호화 해제 후 이를 프로토콜에 맞게 처리하는 로직 추가 필요
                // 암호화 해제 함수
                // 데이터 처리 후
                pSession->ParsePacket();

                
                /* send 테스트용 코드*/
                BYTE tmp[4] = { 0x02,0x00,65,65 };
                pSession->SendPacket(tmp, 4);
                pSession->InitializeReceiver();
                pSession->BindRecv();
            }
        }
        catch (std::exception ex)
        {
            printf("[ERROR] Exception: \n", ex.what());
            if (WSAGetLastError() != 64)
            {
                printf("[Error] GQCS error\n");
            }

            printf("[Error] Remove Client\n");
            if (pSession->IsConnected())
            {
                DeleteSession(pSession);
            }
        }
    }
    printf("[-] Stop WorkerThread\n");
    return uResult;
}