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
	void DestroyRoom();

	template<std::size_t N>
	void SendChat(Session* requestorSession, std::array<BYTE, N> data);

private:
	std::mutex mtx;
	std::wstring RoomName;
	Session* HostSession;
	Session* GuestSession;
};

#endif