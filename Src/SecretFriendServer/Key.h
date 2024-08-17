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
    bool ReceivePublicKey(Session* requestorSession, const std::array<BYTE, RSA_KEY_SIZE>& data);
    bool ReceiveSymmetricKey(Session* requestorSession, const std::array<BYTE, AES_KEY_SIZE>& data);
    bool ReceiveRoomSymmetricKey(Session* requestorSession, const std::array<BYTE, RSA_KEY_SIZE>& data);
    bool SendGuestPublicKey(Session* requestorSession);
    bool SendRoomSymmetricKey(Session* requestorSession, const std::array<BYTE, RSA_KEY_SIZE>& data);

private:
    std::mutex mtx; 
};

#endif