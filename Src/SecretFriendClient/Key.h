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
    std::vector<BYTE> PublicKey;    // 공개 키 (RSA-4096)
    std::vector<BYTE> AESKey;       // 대칭 키
    std::vector<BYTE> AESIV;        // 초기화 벡터
};
