#pragma once

#include <openssl/evp.h>
#include <vector>
#include <string>

class RSAEncryption {
public:
    RSAEncryption();
    ~RSAEncryption();

    std::string exportPublicKeyPEM() const;
    std::string exportPrivateKeyPEM() const;

    std::vector<unsigned char> encryptWithPublicKey(const std::vector<unsigned char>& plaintext) const;
    std::vector<unsigned char> decryptWithPrivateKey(const std::vector<unsigned char>& ciphertext) const;

private:
    EVP_PKEY* pkey;

    void handleCryptoErrors() const;
};
