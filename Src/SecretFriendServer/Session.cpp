#include "Session.h"
#include "Room.h"
#include "RoomList.h"
#include "Key.h"
#include "..\ChatCrypto\AES.h"


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

bool Session::SendPacket(std::vector<BYTE> data, PacketType pType)
{
    // IOCP 워커 쓰레드에서 메모리 할당 해제시킴
    AESWrapper aes;
    std::vector<BYTE> encData = aes.encryptWithAES(data, AESKey, AESIV);
    WSABUF wsabuf = IOData[IO_SEND].SetData(pType, encData.data(), encData.size());
    DWORD dwSentNumBytes = 0;
    int nRet = WSASend(SessionSocket, &wsabuf, 1, &dwSentNumBytes, 0, (LPWSAOVERLAPPED)&IOData[IO_SEND], NULL);

    printf("[sentByte] %d bytes sent\n", dwSentNumBytes);

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

LONGLONG Session::GetSessionID() const
{
    // session id return
    return SessionID;
}

void Session::DecryptPacket()
{
    AESWrapper aes;
    std::vector<BYTE> data = GetPacketData();
    if (data[0] != PacketType::CLIENT_SEND_PUBLICKEY)
    {

    }

    GetPacketData() = aes.decryptWithAES(this->GetPacketData(), this->AESKey, this->AESIV);
}

void Session::ParsePacket()
{
    std::vector<BYTE> data = GetPacketData();
    std::vector<BYTE> response;
    PacketType ptype = GetPacketType();

    AESWrapper aes;
    std::vector<BYTE> decData = aes.decryptWithAES(data, GetAESKey(), GetAESIV());

    if (decData.size() == 0 || data[0] == 0x00)
    {
        printf("Parse fail\n");
        return;
    }

    KeyManager keyManager;

    switch (ptype)
    {
    case CLIENT_SEND_PUBLICKEY:
    {
        // 클라이언트로부터 공개 키 수신
        std::vector<BYTE> publicKey;
        std::copy(data.begin() + 1, data.begin() + 1 + RSA_KEY_SIZE, publicKey.begin());

        keyManager.ReceivePublicKey(this, publicKey);

        std::vector<BYTE> data = { 'O', 'K' };
        SendPacket(data, SERVER_SEND_PARITY);

        break;
    }
    case CLIENT_SEND_SYMMETRICKEY:
    {
        // 클라이언트로부터 대칭 키 수신
        std::vector<BYTE> symmetricKey;
        std::copy(data.begin() + 1, data.begin() + 1 + AES_KEY_SIZE, symmetricKey.begin());

        keyManager.ReceiveSymmetricKey(this, symmetricKey);
        break;
    }

    case CLIENT_SEND_SESSIONID:
        break;

    case CLIENT_REQ_ROOM_LIST:
    {
        std::vector<std::shared_ptr<Room>> list = GetRoomList();
        std::vector<BYTE> data;
        for (int i = 0; i < list.size(); i++)
        {
            std::wstring roomName = list[i]->GetRoomName();
            for (int j = 0; j < roomName.size(); j++)
            {
                data.push_back(roomName[j]);
            }
            data.push_back(0x00);
        }

        SendPacket(data, SERVER_ROOM_LIST);
        break;
    }
    case CLIENT_REQ_ROOM_CREATE:
    {
        std::wstring roomName = std::wstring((PWCHAR)data.data());
        std::shared_ptr<Room> target;
        if (IsRoomExists(roomName, target) == true)
        {
            SendPacket(response, SERVER_REQ_FAIL);
        }
        else
        {
            std::shared_ptr<Room> newRoom = std::make_shared<Room>(roomName, this);
            GetRoomList().push_back(newRoom);
            SessionState = 2;
            JoinedRoom = newRoom;

            SendPacket(response, SERVER_REQ_SUCCESS);
        }

        break;
    }
    case CLIENT_REQ_ROOM_ENTER:
    {
        std::wstring roomName = std::wstring((PWCHAR)data.data());
        std::shared_ptr<Room> target;
        if (IsRoomExists(roomName, target) == true)
        {
            target->JoinRoom(this);
        }
        else
        {
            SendPacket(response, SERVER_REQ_FAIL);
        }
        break;
    }
    case CLIENT_REQ_ROOM_EXIT:
    {
        JoinedRoom->LeaveRoom(this);
        break;
    }
    case CLIENT_REQ_OPPONENT_PUBLIC_KEY:
        keyManager.SendGuestPublicKey(this);
        break;

    case CLIENT_SEND_CHAT:
        JoinedRoom->SendChat(this, data);
        break;

    default:
        break;
    }
}

void Session::StorePublicKey(const std::vector<BYTE>& publicKey)
{
    std::copy(publicKey.begin(), publicKey.end(), PublicKey.begin());
}

void Session::StoreAESKey(const std::vector<BYTE>& aesKey)
{
    std::copy(aesKey.begin(), aesKey.end(), AESKey.begin());
}

std::vector<BYTE> Session::GetPublicKey() const
{
    return PublicKey;
}

std::vector<BYTE> Session::GetAESKey() const
{
    return AESKey;
}

std::vector<BYTE> Session::GetAESIV() const
{
    return AESIV;
}