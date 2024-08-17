#include "Session.h"

void Session::Close()
{
    if (false == IsConnected())
        return;

    struct linger stLinger = { 0, 0 };
    shutdown(SessionSocket, SD_BOTH);
    setsockopt(SessionSocket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
    closesocket(SessionSocket);
    printf("[-] Client disconnected\n");
}

bool Session::BindIOCompletionPort(HANDLE iocpHandle)
{
    // CompletionKey로 해당 Session 객체를 사용한다.
    HANDLE hIOCP = CreateIoCompletionPort((HANDLE)SessionSocket, iocpHandle, (ULONG_PTR)(this), 0);

    if (hIOCP == INVALID_HANDLE_VALUE)
    {
        printf("[ERROR] CreateIoCompletionPort() function: %d\n", GetLastError());
        return false;
    }

    return true;
}

bool Session::Connect(HANDLE iocpHandle)
{
    if (BindIOCompletionPort(iocpHandle) == false)
    {
        printf("[ERROR] BindIOCompletionPort() funtion\n");
        return false;
    }

    if (BindRecv() == false)
    {
        Close();
        return false;
    }

    printf("[+] New client connected\n");

    return true;
}

int Session::recv(WSABUF wsabuf)
{
    DWORD dwFlag = 0;
    DWORD dwRecvNumBytes = 0;

    return WSARecv(SessionSocket, &wsabuf, 1, &dwRecvNumBytes, &dwFlag, (LPWSAOVERLAPPED)&IOData[IO_RECV], NULL);
}

bool Session::BindRecv()
{
    int nRet = this->recv(IOData[IO_RECV].WsaBuf());

    if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
    {
        printf("[RECV_SOCKET_ERROR] Client Connection Terminated\n");
        Close();
        return false;
    }

    return true;
}

bool Session::SendPacket(PBYTE pData, int dataSize)
{
    // IOCP 워커 쓰레드에서 메모리 할당 해제시킴
    IO_DATA* ioData = new IO_DATA(IO_SEND);
    WSABUF wsabuf = ioData->SetData(pData, dataSize);
    DWORD dwSentNumBytes = 0;
    //int nRet = WSASend(SessionSocket, &wsabuf, 1, &dwSentNumBytes, 0, (LPWSAOVERLAPPED)ioData, NULL);
    int nRet = WSASend(SessionSocket, &wsabuf, 1, &dwSentNumBytes, 0, (LPWSAOVERLAPPED)&IOData[IO_SEND], NULL);

    printf("[sentByte] %d\n", dwSentNumBytes);

    if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
    {
        printf("[ERROR] client Send Error\n");
        return false;
    }

    return true;
}

int Session::send(WSABUF wsabuf)
{
    DWORD dwSentNumBytes = 0;

    return WSASend(SessionSocket, &wsabuf, 1, &dwSentNumBytes, 0, (LPWSAOVERLAPPED)&IOData[IO_SEND], NULL);
}

LONGLONG Session::GetSessionID()
{
    // session id return
    return SessionID;
}