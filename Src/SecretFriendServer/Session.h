#ifndef SESSION_H
#define SESSION_H

#include <array>
#include "IoData.h"
#include "Packet.h"

constexpr int IO_DATA_SIZE = 2;
constexpr int RSA_KEY_SIZE = 4096;
constexpr int AES_KEY_SIZE = 256;

// PacketBuilder를 상속받음
struct Session : public PacketBuilder
{
public:
	Session(SOCKET sock) :
		IOData({ IO_RECV, IO_SEND }), SessionSocket(sock)
	{
		SessionState = 0;
		PublicKey.fill(0);
		AESKey.fill(0);
	}

	Session(Session const&) = delete;
	Session(Session const&&) = delete;
	Session& operator=(Session const&) = delete;
	Session& operator=(Session&&) = delete;

	~Session() {}

	void Close();
	bool IsConnected() const { return SessionSocket != INVALID_SOCKET; }
	bool Connect(HANDLE iocpHandle);
	bool BindRecv();
	bool SendPacket(PBYTE pData, int dataSize);

private:
	int recv(WSABUF wsabuf);
	int send(WSABUF wsabuf);

	bool BindIOCompletionPort(HANDLE iocpHandle);

	SOCKET SessionSocket;
	DWORD SessionState;							// 현재 세션이 위치한 곳 (키교환중(0), 로비(1) 또는 채팅방(2))
	LONGLONG SessionID;
	std::array<IO_DATA, IO_DATA_SIZE> IOData;	// Send, Recieve 두개를 가진다
	std::array<BYTE, RSA_KEY_SIZE> PublicKey;	// 상대방과 키교환을 위해 사용하는 공개키 (RSA-4096)
	// TODO: PublicKey는 별도의 User
	std::array<BYTE, AES_KEY_SIZE> AESKey;		// 해당 세션과 통신할 때 사용하는 암호화 키
	std::array<BYTE, AES_KEY_SIZE> AESIV;		// 해당 세션과 통신할 때 사용하는 초기화 벡터
};

#endif