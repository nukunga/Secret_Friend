#pragma once
#include "Packet.h"
#include <winsock2.h>
#include "../ChatCrypto/RSA.h"

constexpr int IO_DATA_SIZE = 2;
constexpr int RSA_KEY_SIZE = 4096;
constexpr int AES_KEY_SIZE = 256;

class Network : public PacketBuilder
{

public:
	//void send(hSocket, message.c_str(), message.size() + 1, 0); 

	Network(SOCKET sock) :
		IOData({ IO_RECV, IO_SEND }), SessionSocket(sock), rsa() // 4096-bit RSA Ű ����
	{
		PublicKey.resize(256, 0);
		AESKey.resize(256, 0);
		CreatePublicKey();
		CreatePrivateKey();
		CreateSymmetricKey();
		CreateIV();
	}

	Network(Network const&) = delete;
	Network(Network const&&) = delete;
	Network& operator=(Network const&) = delete;
	Network& operator=(Network&&) = delete;

	~Network() {}


	std::vector<BYTE> RoomList;


	void CreatePublicKey();
	void CreateSymmetricKey();
	void CreatePrivateKey();
	void CreateIV();

	void Close();
	bool IsConnected() const { return SessionSocket != INVALID_SOCKET; }
	bool BindRecv();

	bool SendPacket(std::vector<BYTE> data, PacketType pType);
	void ParsePacket();

	bool SendPublicKey();
	bool SendSymmetricKey();

	void PrintPrivateKey();
	void PrintPublicKey();

private:
	int recv(WSABUF wsabuf);
	int send(WSABUF wsabuf);

	SOCKET SessionSocket;
	std::array<IO_DATA, IO_DATA_SIZE> IOData;	// Send, Recieve �ΰ��� ������
	std::vector<BYTE> PublicKey;	// Host�� ��� Guest�� ����Ű
	std::vector<BYTE> PrivateKey;
	std::vector<BYTE> AESKey;		// �ش� ���ǰ� ����� �� ����ϴ� ��ȣȭ Ű
	std::vector<BYTE> AESIV;		// �ش� ���ǰ� ����� �� ����ϴ� �ʱ�ȭ ����

	RSAEncryption rsa; // RSA ��ȣȭ ��ü
};