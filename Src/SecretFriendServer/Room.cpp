#include "Room.h"

bool Room::JoinRoom(Session* requestorSession)
{
	// 0이 아닌 경우 누군가 들어가 있기 때문에 연결을 거부한다.
	mtx.lock();
	if (GuestSession != 0)
	{
		mtx.unlock();
		return false;
	}
	else
		GuestSession = requestorSession;
	mtx.unlock();

	// TODO: 키교환 진행
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

		std::vector<BYTE> test = { 0x00, 0x00, 0x00, 0x00, 0x00 };
		GuestSession->SendPacket(test);
		//HostSession->SendPacket();
		// TODO: Send leave packet to both clients
	}
}

void Room::DestroyRoom()
{
	LeaveRoom(GuestSession);
	// TODO: ...
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