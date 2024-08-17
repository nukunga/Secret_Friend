#pragma once
#ifndef KEY_H
#define KEY_H

#include <vector>
#include <mutex> 
#include "Session.h"

class KeyManager
{
public:
    // 수신된 공개 키를 세션에 저장
    bool ReceivePublicKey(Session* requestorSession, const std::vector<BYTE>& data);
    bool ReceiveSymmetricKey(Session* requestorSession, const std::vector<BYTE>& data);
    bool ReceiveRoomSymmetricKey(Session* requestorSession, const std::vector<BYTE>& data);
    bool SendGuestPublicKey(Session* requestorSession);
    bool SendRoomSymmetricKey(Session* requestorSession, const std::vector<BYTE>& data);

private:
    std::mutex mtx; 
};

#endif