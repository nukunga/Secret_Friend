#include "Room.h"

bool Room::JoinRoom(Session* requestorSession)
{
	std::vector<BYTE> packet;

	// 0�� �ƴ� ��� ������ �� �ֱ� ������ ������ �ź��Ѵ�.
	mtx.lock();
	if (GuestSession != 0)
	{
		mtx.unlock();
		GuestSession->SendPacket(packet, SERVER_REQ_FAIL);
		return false;
	}
	else
		GuestSession = requestorSession;
	mtx.unlock();

	// TODO: Ű��ȯ ����
	
	HostSession->SendPacket(packet, SERVER_JOIN_NOTIFY);

	packet.clear();
	GuestSession->SendPacket(packet, SERVER_REQ_SUCCESS);

	return true;
}

void Room::LeaveRoom(Session* requestorSession)
{
	Session* requestor = requestorSession;
	std::vector<BYTE> packet;

	if (requestor == HostSession)
	{
		// �����ڿ��� ���� �������� �˸�
		GuestSession->SendPacket(packet, SERVER_ROOM_DESTROY_NOTIFY);
		packet.clear();
		// TODO: ���� ���� ����
		
		// ���忡�� �� ������ �������� �˸�
		GuestSession->SendPacket(packet, SERVER_REQ_SUCCESS);
		// TODO: ���� ���� ����
	}
	else if (requestor == GuestSession)
	{
		mtx.lock();
		GuestSession = 0;
		mtx.unlock();

		GuestSession->SendPacket(packet, SERVER_REQ_SUCCESS);
		packet.clear();
		// TODO: ���� ���º���

		HostSession->SendPacket(packet, SERVER_LEAVE_NOTIFY);
	}
}

void Room::SendChat(Session* requestorSession, std::vector<BYTE> data)
{
	if (requestorSession == HostSession)
	{
		GuestSession->SendPacket(data, SERVER_SEND_CHAT);
	}
	else if (requestorSession == GuestSession)
	{
		HostSession->SendPacket(data, SERVER_SEND_CHAT);
	}
}