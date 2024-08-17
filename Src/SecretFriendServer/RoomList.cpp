#include "RoomList.h"
#include "Room.h"

std::mutex roomMtx;
std::vector<std::shared_ptr<Room>> RoomList;

std::vector<std::shared_ptr<Room>> GetRoomList()
{
    return RoomList;
}

void AddRoom(std::shared_ptr<Room> room)
{
    roomMtx.lock();
    RoomList.push_back(std::move(room));
    roomMtx.unlock();
}

void RemoveRoom(std::shared_ptr<Room> room)
{
    roomMtx.lock();
    auto it = std::find(RoomList.begin(), RoomList.end(), room);
    RoomList.erase(it);
    roomMtx.unlock();
}

bool IsRoomExists(std::wstring name, std::shared_ptr<Room>& output)
{
    roomMtx.lock();
    for (int i = 0; i < RoomList.size(); i++)
    {
        if (RoomList[i]->GetRoomName() == name)
        {
            output = RoomList[i];
            roomMtx.unlock();
            return true;
        }
    }
    roomMtx.unlock();

    return false;
}
