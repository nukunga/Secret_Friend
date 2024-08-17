#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <mutex>
#include "Session.h"


class Room
{
public:
	Room(std::wstring name, LONGLONG hostSessionID)
	{
		RoomName = name;
		HostSessionID = hostSessionID;
		GuestSessionID = 0;
	}

	bool JoinRoom(Session requestorSession);
	bool LeaveRoom(LONGLONG guestSessionID);
	bool DestroyRoom();
	bool SendData(LONGLONG senderID, std::array<BYTE, SOCKET_BUFFER_SIZE> data, DWORD dataSize);

private:
	std::mutex mtx;
	std::wstring RoomName;
	LONGLONG HostSessionID;
	LONGLONG GuestSessionID;
};

#endif