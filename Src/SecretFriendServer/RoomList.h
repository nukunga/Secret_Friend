#pragma once

#ifndef ROOMLIST_H
#define ROOMLIST_H

#include <vector>
#include <string>
#include <mutex>
#include <winsock2.h>

class Room;

class RoomList
{
public:
	std::vector<std::shared_ptr<Room>> GetRoomList();
	void AddRoom(std::shared_ptr<Room> room);
	void RemoveRoom(std::shared_ptr<Room> room);

private:
	std::mutex mtx;
	std::vector<std::shared_ptr<Room>> RoomList;
};
#endif