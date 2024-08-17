#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <winsock2.h>

class Key 
{
public:

    void StorePublicKey(const std::vector<BYTE>& publicKey);
    void StoreAESKey(const std::vector<BYTE>& aesKey);
    std::vector<BYTE> GetPublicKey() const;

private:
    std::vector<BYTE> PublicKey;    // ���� Ű (RSA-4096)
    std::vector<BYTE> AESKey;       // ��Ī Ű
    std::vector<BYTE> AESIV;        // �ʱ�ȭ ����
};
