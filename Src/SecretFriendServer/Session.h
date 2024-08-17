#ifndef SESSION_H
#define SESSION_H

#include <array>
#include <vector>
#include "Packet.h"

constexpr int IO_DATA_SIZE = 2;
constexpr int RSA_KEY_SIZE = 4096;
constexpr int AES_KEY_SIZE = 256;

class Room;

// PacketBuilder�� ��ӹ���
struct Session : public PacketBuilder
{
public:
	Session(SOCKET sock) :
		IOData({ IO_RECV, IO_SEND }), SessionSocket(sock)
	{
		SessionState = 0;
		PublicKey.resize(256, 0);
		AESKey.resize(256, 0);
	}

	Session(Session const&) = delete;
	Session(Session const&&) = delete;
	Session& operator=(Session const&) = delete;
	Session& operator=(Session&&) = delete;

	~Session() {}

	void Close();
	bool IsConnected() const { return SessionSocket != INVALID_SOCKET; }
	std::shared_ptr<Room> GetJoinedRoom() const { return JoinedRoom; }
	bool Connect(HANDLE iocpHandle);
	bool BindRecv();

	bool SendPacket(std::vector<BYTE> data);
	void ParsePacket();

	LONGLONG GetSessionID() const;

	void StorePublicKey(const std::vector<BYTE>& publicKey);
	void StoreAESKey(const std::vector<BYTE>& publicKey);
	std::vector<BYTE> GetPublicKey() const;


private:
	int recv(WSABUF wsabuf);
	int send(WSABUF wsabuf);

	bool BindIOCompletionPort(HANDLE iocpHandle);

	SOCKET SessionSocket;
	DWORD SessionState;							// ���� ������ ��ġ�� �� (Ű��ȯ��(0), �κ�(1) �Ǵ� ä�ù�(2))
	std::shared_ptr<Room> JoinedRoom;
	LONGLONG SessionID;
	std::array<IO_DATA, IO_DATA_SIZE> IOData;	// Send, Recieve �ΰ��� ������
	std::vector<BYTE> PublicKey;	// ����� Ű��ȯ�� ���� ����ϴ� ����Ű (RSA-4096)
	// TODO: PublicKey�� ������ User
	std::vector<BYTE> AESKey;		// �ش� ���ǰ� ����� �� ����ϴ� ��ȣȭ Ű
	std::vector<BYTE> AESIV;		// �ش� ���ǰ� ����� �� ����ϴ� �ʱ�ȭ ����
};

#endif