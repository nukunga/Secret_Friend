#include "Room.h"

bool Room::JoinRoom(Session* requestorSession)
{
	std::vector<BYTE> packet;

	// 0이 아닌 경우 누군가 들어가 있기 때문에 연결을 거부한다.
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

	// TODO: 키교환 진행
	
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
		// 참여자에게 방이 없어짐을 알림
		GuestSession->SendPacket(packet, SERVER_ROOM_DESTROY_NOTIFY);
		packet.clear();
		// TODO: 세션 상태 변경
		
		// 방장에게 방 삭제에 성공함을 알림
		GuestSession->SendPacket(packet, SERVER_REQ_SUCCESS);
		// TODO: 세션 상태 변경
	}
	else if (requestor == GuestSession)
	{
		mtx.lock();
		GuestSession = 0;
		mtx.unlock();

		GuestSession->SendPacket(packet, SERVER_REQ_SUCCESS);
		packet.clear();
		// TODO: 세션 상태변경

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