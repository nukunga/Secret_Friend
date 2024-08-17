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
    bool ReceivePublicKey(Session* requestorSession, const std::array<BYTE, RSA_KEY_SIZE>& data);
    bool ReceiveSymmetricKey(Session* requestorSession, const std::array<BYTE, AES_KEY_SIZE>& data);
    bool ReceiveRoomSymmetricKey(Session* requestorSession, const std::array<BYTE, RSA_KEY_SIZE>& data);
    bool SendGuestPublicKey(Session* requestorSession);
    bool SendRoomSymmetricKey(Session* requestorSession, const std::array<BYTE, RSA_KEY_SIZE>& data);

private:
    std::mutex mtx; 
};

#endif