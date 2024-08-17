#ifndef SESSION_H
#define SESSION_H

#include <array>
#include "IoData.h"
#include "Packet.h"

constexpr int IO_DATA_SIZE = 2;
constexpr int RSA_KEY_SIZE = 4096;
constexpr int AES_KEY_SIZE = 256;

// PacketBuilder�� ��ӹ���
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
	DWORD SessionState;							// ���� ������ ��ġ�� �� (Ű��ȯ��(0), �κ�(1) �Ǵ� ä�ù�(2))
	LONGLONG SessionID;
	std::array<IO_DATA, IO_DATA_SIZE> IOData;	// Send, Recieve �ΰ��� ������
	std::array<BYTE, RSA_KEY_SIZE> PublicKey;	// ����� Ű��ȯ�� ���� ����ϴ� ����Ű (RSA-4096)
	// TODO: PublicKey�� ������ User
	std::array<BYTE, AES_KEY_SIZE> AESKey;		// �ش� ���ǰ� ����� �� ����ϴ� ��ȣȭ Ű
	std::array<BYTE, AES_KEY_SIZE> AESIV;		// �ش� ���ǰ� ����� �� ����ϴ� �ʱ�ȭ ����
};

#endif