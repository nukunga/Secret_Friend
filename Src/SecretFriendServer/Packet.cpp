#include "Packet.h"

void PacketBuilder::PushRecvPacket(IO_DATA ioData, size_t ioSize)
{
    Mtx.lock();
    for (int i = 0; i < ioSize; i++)
    {
        PacketQueue.push(ioData.WsaBuf().buf[i]);
    }
    Mtx.unlock();
}

void PacketBuilder::ValidatePacket()
{
    if (ValidateHeader() == true)
    {
        if (DataSize <= PacketQueue.size())
        {
            auto packet = std::make_unique<BYTE[]>(DataSize);
            Mtx.lock();
            for (int i = 0; i < DataSize; i++)
            {
                packet[i] = PacketQueue.front();
                PacketQueue.pop();
            }

            Hdr.fill(0);
            DataSize = 0;
            Mtx.unlock();
            
            // TODO: 정상 패킷 수신 후 패킷을 여기서 처리한다.
            printf("Parse success: %s\n", packet.get());
        }
    }
    else
    {
        printf("Parse fail\n");
    }
}

bool PacketBuilder::ValidateHeader()
{
    // 올바른 패킷 헤더를 받을 때 까지 큐에서 pop
    std::array<BYTE, HDR_FOOTER_SIZE> correctHdr = { 0x24, 0x08, 0x01, 0x05 };
    Mtx.lock();
    while (Hdr != correctHdr && PacketQueue.size() >= 4)
    {
        Hdr.fill(0);

        Hdr.at(0) = PacketQueue.front();
        PacketQueue.pop();
        if (Hdr.at(0) != 0x24) continue;
        Hdr.at(1) = PacketQueue.front();
        PacketQueue.pop();
        if (Hdr.at(1) != 0x08) continue;
        Hdr.at(2) = PacketQueue.front();
        PacketQueue.pop();
        if (Hdr.at(2) != 0x01) continue;
        Hdr.at(3) = PacketQueue.front();
        PacketQueue.pop();
        if (Hdr.at(3) != 0x05) continue;
    }
    Mtx.unlock();

    if (Hdr != correctHdr)
        return false;

    // 올바른 패킷 헤더를 받았다면 패킷 데이터 크기 파싱
    if (DataSize == 0 && PacketQueue.size() >= 2)
    {
        std::array<BYTE, HDR_SIZE_DEF> size;
        Mtx.lock();
        size[0] = PacketQueue.front(); PacketQueue.pop();
        size[1] = PacketQueue.front(); PacketQueue.pop();
        DataSize = *((PWORD)size.data());

        // 정상 패킷은 데이터 크기가 0이 될 수 없음
        if (DataSize == 0 || DataSize > 5000)
        {
            Hdr.fill(0);
            DataSize = 0;
            Mtx.unlock();
            return false;
        }

        Mtx.unlock();
    }

    return true;
}

void PacketBuilder::ParsePacket()
{
   
}