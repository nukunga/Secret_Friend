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
	std::shared_ptr<Room> GetJoinedRoom() const { return JoinedRoom; }
	bool Connect(HANDLE iocpHandle);
	bool BindRecv();

	bool SendPacket(std::vector<BYTE> data);
	void ParsePacket();

	LONGLONG GetSessionID() const;

	void StorePublicKey(const std::array<BYTE, RSA_KEY_SIZE>& publicKey);
	void StoreAESKey(const std::array<BYTE, AES_KEY_SIZE>& publicKey);
	std::array<BYTE, RSA_KEY_SIZE> GetPublicKey() const;


private:
	int recv(WSABUF wsabuf);
	int send(WSABUF wsabuf);

	bool BindIOCompletionPort(HANDLE iocpHandle);

	SOCKET SessionSocket;
	DWORD SessionState;							// ���� ������ ��ġ�� �� (Ű��ȯ��(0), �κ�(1) �Ǵ� ä�ù�(2))
	std::shared_ptr<Room> JoinedRoom;
	LONGLONG SessionID;
	std::array<IO_DATA, IO_DATA_SIZE> IOData;	// Send, Recieve �ΰ��� ������
	std::array<BYTE, RSA_KEY_SIZE> PublicKey;	// ����� Ű��ȯ�� ���� ����ϴ� ����Ű (RSA-4096)
	// TODO: PublicKey�� ������ User
	std::array<BYTE, AES_KEY_SIZE> AESKey;		// �ش� ���ǰ� ����� �� ����ϴ� ��ȣȭ Ű
	std::array<BYTE, AES_KEY_SIZE> AESIV;		// �ش� ���ǰ� ����� �� ����ϴ� �ʱ�ȭ ����
};

#endif