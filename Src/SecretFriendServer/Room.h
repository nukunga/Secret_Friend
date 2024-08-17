#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <mutex>
#include "Session.h"

class Room
{
public:
	Room(std::wstring name, Session* hostSession)
	{
		RoomName = name;
		HostSession = hostSession;
		GuestSession = 0;
	}

	bool JoinRoom(Session* requestorSession);
	void LeaveRoom(Session* requestorSession);
	void SendChat(Session* requestorSession, std::vector<BYTE> data);
	std::wstring GetRoomName() const { return RoomName; }

private:
	std::mutex mtx;
	std::wstring RoomName;
	Session* HostSession;
	Session* GuestSession;
};

#endif