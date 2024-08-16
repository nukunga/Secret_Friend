#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

#include "IOData.h"
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
	CLIENT_REQ_PUBLICKEY = 100,
	CLIENT_SEND_NICKNAME = 101,

	CLIENT_REQ_ROOM_LIST = 110,
	CLIENT_REQ_ROOM_CREATE = 111,
	CLIENT_REQ_ROOM_ENTER = 112,
	CLIENT_REQ_ROOM_EXIT = 113,
	CLIENT_REQ_OPPONENT_PUBLIC_KEY = 114,

	CLIENT_SEND_CHAT = 120,

	/* From Server */
	SERVER_REQ_SUCCESS = 200,
	SERVER_REQ_FAIL = 201,
	SERVER_PUBLIC_KEY = 202,

	SERVER_ROOM_LIST = 210,
	SERVER_ROOM_INFO = 211,
	SERVER_OPPONENT_PUBLIC_KEY = 212,

	SERVER_SEND_CHAT = 220
};

class PacketBuilder
{
public:
	PacketBuilder()
	{
		Hdr.fill(0);
		DataSize = 0;
		PType = E_PK_UNDEFINED;
	}

	~PacketBuilder()
	{}

	void PushRecvPacket(IO_DATA ioData, size_t ioSize);
	void ValidatePacket();
	void ParsePacket();

private:
	bool ValidateHeader();

	std::queue<BYTE> PacketQueue;			// ��Ŷ ť�� ���� �� ������� ������ ó��
	std::array<BYTE, HDR_FOOTER_SIZE> Hdr;	// ���� ��Ŷ ��ü�� ��� ��ȿ�� �˻�� �ñ״��� 0x24, 0x08, 0x01, 0x05
	WORD DataSize = 0;						// ���� ��Ŷ ��ü���� ������ ��Ŷ ũ��
	PacketType PType;						// ��Ŷ ���� ����
	std::mutex Mtx;							// Mutual exclusion ������ ���� ���ؽ� (���Ǻ��� �����ȴ�)
};

#endif