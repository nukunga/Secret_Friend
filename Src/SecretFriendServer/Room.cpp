#include "Room.h"

bool Room::JoinRoom(Session requestorSession)
{
	// 0�� �ƴ� ��� ������ �� �ֱ� ������ ������ �ź��Ѵ�.
	mtx.lock();
	if (GuestSessionID != 0)
		return false;
	else
		GuestSessionID = requestorSession.SessionID;
	mtx.unlock();



	// TODO: Ű��ȯ ����

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