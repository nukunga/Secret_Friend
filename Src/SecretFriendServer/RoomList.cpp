#include "RoomList.h"
#include "Room.h"

std::vector<std::shared_ptr<Room>> RoomList::GetRoomList()
{
    return RoomList;
}

void RoomList::AddRoom(std::shared_ptr<Room> room)
{
    mtx.lock();
    RoomList.push_back(std::move(room));
    mtx.unlock();
}

void RoomList::RemoveRoom(std::shared_ptr<Room> room)
{
    mtx.lock();
    auto it = std::find(RoomList.begin(), RoomList.end(), room);
    RoomList.erase(it);
    mtx.unlock();
}
