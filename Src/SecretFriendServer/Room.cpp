#include "Room.h"

bool Room::JoinRoom(Session requestorSession)
{
	// 0이 아닌 경우 누군가 들어가 있기 때문에 연결을 거부한다.
	mtx.lock();
	if (GuestSessionID != 0)
		return false;
	else
		GuestSessionID = requestorSession.SessionID;
	mtx.unlock();



	// TODO: 키교환 진행

}

bool Room::LeaveRoom(Session requestorSession)
{
	if (requestorSession.GetSessionID() == )
}

bool Room::DestroyRoom()
{
	LeaveRoom(GuestSessionID);
	// TODO: ...
}

bool Room::SendData(LONGLONG senderID, std::array<BYTE, SOCKET_BUFFER_SIZE> data, DWORD dataSize)
{

}