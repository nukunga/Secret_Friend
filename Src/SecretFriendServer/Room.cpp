#include "Room.h"

bool Room::JoinRoom(Session* requestorSession)
{
	// 0�� �ƴ� ��� ������ �� �ֱ� ������ ������ �ź��Ѵ�.
	mtx.lock();
	if (GuestSession != 0)
	{
		mtx.unlock();
		return false;
	}
	else
		GuestSession = requestorSession;
	mtx.unlock();

	// TODO: Ű��ȯ ����
	return true;
}

void Room::LeaveRoom(Session* requestorSession)
{
	Session* requestor = requestorSession;
	if (requestor == HostSession)
	{
		// TODO: Destroy Room
	}
	else if (requestor == GuestSession)
	{
		mtx.lock();
		GuestSession = 0;
		mtx.unlock();

		GuestSession->SendPacket(TEST);
		HostSession->SendPacket();
		// TODO: Send leave packet to both clients
	}
}

void Room::DestroyRoom()
{
	LeaveRoom(GuestSessionID);
	// TODO: ...
}

template<std::size_t N>
void Room::SendChat(Session* requestorSession, std::array<BYTE, N> data)
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