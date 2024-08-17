#include "Room.h"

bool Room::JoinRoom(Session* requestorSession)
{
	std::vector<BYTE> packet;

	// 0이 아닌 경우 누군가 들어가 있기 때문에 연결을 거부한다.
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

	// TODO: 키교환 진행
	
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
		// 참여자에게 방이 없어짐을 알림
		packet.push_back(PacketType::SERVER_ROOM_DESTROY_NOTIFY);
		GuestSession->SendPacket(packet);
		packet.clear();
		// TODO: 세션 상태 변경
		
		// 방장에게 방 삭제에 성공함을 알림
		packet.push_back(PacketType::SERVER_REQ_SUCCESS);
		GuestSession->SendPacket(packet);
		// TODO: 세션 상태 변경
	}
	else if (requestor == GuestSession)
	{
		mtx.lock();
		GuestSession = 0;
		mtx.unlock();

		packet.push_back(PacketType::SERVER_REQ_SUCCESS);
		GuestSession->SendPacket(packet);
		packet.clear();
		// TODO: 세션 상태변경

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