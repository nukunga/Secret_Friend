#pragma once
#ifndef KEY_H
#define KEY_H

#include <vector>
#include <mutex> 
#include "Session.h"

class KeyManager
{
public:
    // ���ŵ� ���� Ű�� ���ǿ� ����
    bool ReceivePublicKey(Session* requestorSession, const std::vector<BYTE>& data);
    bool ReceiveSymmetricKey(Session* requestorSession, const std::vector<BYTE>& data);
    bool ReceiveRoomSymmetricKey(Session* requestorSession, const std::vector<BYTE>& data);
    bool SendGuestPublicKey(Session* requestorSession);
    bool SendRoomSymmetricKey(Session* requestorSession, const std::vector<BYTE>& data);

private:
    std::mutex mtx; 
};

#endif