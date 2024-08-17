#include "Room.h"

bool Room::JoinRoom(Session* requestorSession)
{
	std::vector<BYTE> packet;

	// 0�� �ƴ� ��� ������ �� �ֱ� ������ ������ �ź��Ѵ�.
	mtx.lock();
	if (GuestSession != 0)
	{
		mtx.unlock();
		packet.push_back(PacketType::SERVER_REQ_FAIL);
		GuestSession->SendPacket(packet);
		return false;
	}
	else
		GuestSession = requestorSession;
	mtx.unlock();

	// TODO: Ű��ȯ ����
	
	packet.push_back(PacketType::SERVER_JOIN_NOTIFY);
	HostSession->SendPacket(packet);

	packet.clear();
	packet.push_back(PacketType::SERVER_REQ_SUCCESS);
	GuestSession->SendPacket(packet);

	return true;
}

void Room::LeaveRoom(Session* requestorSession)
{
	Session* requestor = requestorSession;
	std::vector<BYTE> packet;

	if (requestor == HostSession)
	{
		// �����ڿ��� ���� �������� �˸�
		packet.push_back(PacketType::SERVER_ROOM_DESTROY_NOTIFY);
		GuestSession->SendPacket(packet);
		packet.clear();
		// TODO: ���� ���� ����
		
		// ���忡�� �� ������ �������� �˸�
		packet.push_back(PacketType::SERVER_REQ_SUCCESS);
		GuestSession->SendPacket(packet);
		// TODO: ���� ���� ����
	}
	else if (requestor == GuestSession)
	{
		mtx.lock();
		GuestSession = 0;
		mtx.unlock();

		packet.push_back(PacketType::SERVER_REQ_SUCCESS);
		GuestSession->SendPacket(packet);
		packet.clear();
		// TODO: ���� ���º���

		packet.push_back(PacketType::SERVER_LEAVE_NOTIFY);
		HostSession->SendPacket(packet);
	}
}

void Room::SendChat(Session* requestorSession, std::vector<BYTE> data)
{
	// Set flag to SERVER_SEND_CHAT
	data[0] = PacketType::SERVER_SEND_CHAT;
	if (requestorSession == HostSession)
	{
		GuestSession->SendPacket(data);
	}
	else if (requestorSession == GuestSession)
	{
		HostSession->SendPacket(data);
	}
}