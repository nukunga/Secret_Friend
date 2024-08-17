#pragma once

#ifndef ROOMLIST_H
#define ROOMLIST_H

#include <vector>
#include <string>
#include <mutex>
#include <winsock2.h>

class Room;

std::vector<std::shared_ptr<Room>> GetRoomList();
void AddRoom(std::shared_ptr<Room> room);
void RemoveRoom(std::shared_ptr<Room> room);
bool IsRoomExists(std::wstring name, std::shared_ptr<Room>& output);
#endif