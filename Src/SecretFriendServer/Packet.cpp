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
            
            // TODO: ���� ��Ŷ ���� �� ��Ŷ�� ���⼭ ó���Ѵ�.
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
    // �ùٸ� ��Ŷ ����� ���� �� ���� ť���� pop
    std::array<BYTE, HDR_FOOTER_SIZE> correctHdr = { 0x24, 0x08, 0x01, 0x05 };
    while (Hdr != correctHdr && PacketQueue.size() >= 4)
    {
        Mtx.lock();
        Hdr.fill(0);
        for (int i = 0; i < 4; i++)
        {
            Hdr.at(i) = PacketQueue.front();
            PacketQueue.pop();
        }
        Mtx.unlock();
    }

    if (Hdr != correctHdr)
        return false;

    // �ùٸ� ��Ŷ ����� �޾Ҵٸ� ��Ŷ ������ ũ�� �Ľ�
    if (DataSize == 0 && PacketQueue.size() >= 2)
    {
        std::array<BYTE, HDR_SIZE_DEF> size;
        Mtx.lock();
        size[0] = PacketQueue.front(); PacketQueue.pop();
        size[1] = PacketQueue.front(); PacketQueue.pop();
        DataSize = *((PWORD)size.data());

        // ���� ��Ŷ�� ������ ũ�Ⱑ 0�� �� �� ����
        if (DataSize == 0)
        {
            Hdr.fill(0);
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