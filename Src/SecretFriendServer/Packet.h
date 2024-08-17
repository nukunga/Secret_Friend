#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

#include "IOData.h"
#include "Key.h"
#include <queue>
#include <mutex>

constexpr int HDR_FOOTER_SIZE = 4;
constexpr int HDR_SIZE_DEF = 2;

/*
*			BoBChat ��Ŷ ����
* 
* ==== �� �κ��� ��ȣȭ���� ���� ====
* 0x00~0x03 (4B) = ��Ŷ Ǫ�� (0x24, 0x08, 0x01, 0x05)
* 0x04~0x05 (2B) = ��Ŷ ������ ũ��
* ==== �� �κ��� ��ȣȭ���� ���� ====
* 
* 
* 
* ==== �� �κ��� ����<->Ŭ���̾�Ʈ Ű�� ��ȣȭ �Ǿ���� ====
* 0x06 (1B) = PacketType (��Ŷ ����)
* ~... = ��Ŷ �������� ������ ������
* ==== �� �κ��� ����<->Ŭ���̾�Ʈ Ű�� ��ȣȭ �Ǿ���� ====
* ex) ������� ������ ä���� ������ Ű�� ��ȣȭ�� �ȴ�. �ش� ��� ���� ����<->Ŭ���̾�Ʈ Ű�� ��ȣȭ�� �ȴ�.
* ex) ���� ������ �̸� ���Ž� ����<->Ŭ���̾�Ʈ Ű�� ��ȣȭ �� ä�ù濡�� ��ȯ�� Ű�� ��ȣȭ, �� 2�� ��ȣȭ�� �Ѵ�.
*/

enum PacketType
{
	E_PK_UNDEFINED = 0,

	/* From Client */
	CLIENT_SEND_PUBLICKEY = 100, // CLIENT�� ����Ű ����
	CLIENT_SEND_SYMMETRICKEY = 101, // CLIENT�� ��ĪŰ ����
	CLIENT_SEND_SESSIONID = 102, // SESSIOINID ����
	
	CLIENT_REQ_ROOM_LIST = 110, // ROOM LIST ��û
	CLIENT_REQ_ROOM_CREATE = 111, // ROOM ���� ��û
	CLIENT_REQ_ROOM_ENTER = 112, // ROOM ���� ��û
	CLIENT_REQ_ROOM_EXIT = 113, // ROOM ���� ��û
	CLIENT_REQ_OPPONENT_PUBLIC_KEY = 114, // ������ ����Ű ��û

	CLIENT_SEND_CHAT = 120, // �޼��� ����

	/* From Server */
	SERVER_REQ_SUCCESS = 200, // ���� ���� 
	SERVER_REQ_FAIL = 201, // ���� ����
	SERVER_PUBLIC_KEY = 202, // SERVER�� ����Ű

	SERVER_ROOM_LIST = 210, // SERVER�� ROOM LIST
	SERVER_ROOM_INFO = 211, // SERVER�� ROOM ����
	SERVER_SEND_OPPONENT_PUBLIC_KEY = 212, // SERVER���� Guest�� ����Ű�� ����
	SERVER_SEND_ROOM_SYMMETRICKEY = 213,

	SERVER_SEND_CHAT = 220, // SERVER������ �޼��� ����
	SERVER_JOIN_NOTIFY = 221,
	SERVER_LEAVE_NOTIFY = 222,
	SERVER_ROOM_DESTROY_NOTIFY = 223,

};

class PacketBuilder
{
public:
	PacketBuilder()
	{
		Hdr.fill(0);
		Data.fill(0);
		DataSize = 0;
		PType = E_PK_UNDEFINED;
	}

	~PacketBuilder()
	{}

	void PushRecvPacket(IO_DATA ioData, size_t ioSize);
	void ValidatePacket();
	void ParsePacket();
	void InitializeReceiver();

private:
	bool ValidateHeader();

	std::queue<BYTE> PacketQueue;			// ��Ŷ ť�� ���� �� ������� ������ ó��
	std::array<BYTE, HDR_FOOTER_SIZE> Hdr;	// ���� ��Ŷ ��ü�� ��� ��ȿ�� �˻�� �ñ״��� 0x24, 0x08, 0x01, 0x05
	std::array<BYTE, SOCKET_BUFFER_SIZE> Data;	// ���� �Ľ��� ���� ��Ŷ
	WORD DataSize = 0;						// ���� ��Ŷ ��ü���� ������ ��Ŷ ũ��
	PacketType PType;						// ��Ŷ ���� ����
	std::mutex Mtx;							// Mutual exclusion ������ ���� ���ؽ� (���Ǻ��� �����ȴ�)
};

#endif