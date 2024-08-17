#pragma once

#include <openssl/evp.h>
#include <vector>
#include <string>

class AESWrapper {
public:
    AESWrapper();
    ~AESWrapper();

    std::vector<unsigned char> generateKey();
    std::vector<unsigned char> generateIV();

    std::vector<unsigned char> encryptWithAES(const std::vector<unsigned char>& plaintext,
        const std::vector<unsigned char>& key,
        const std::vector<unsigned char>& iv);

    std::vector<unsigned char> decryptWithAES(const std::vector<unsigned char>& ciphertext,
        const std::vector<unsigned char>& key,
        const std::vector<unsigned char>& iv);

private:
    void handleCryptoErrors() const; // 함수 선언 추가
};
