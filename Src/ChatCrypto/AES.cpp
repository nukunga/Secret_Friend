#include "pch.h"
#include "AES.h"
#include <openssl/rand.h>
#include <openssl/err.h>
#include <iostream>
#include <stdexcept>

AESWrapper::AESWrapper() {
    // OpenSSL 초기화는 자동으로 처리되므로 추가 설정이 필요 없습니다.
}

AESWrapper::~AESWrapper() {
    // OpenSSL 리소스 해제는 자동으로 처리되므로 추가 설정이 필요 없습니다.
}

std::vector<unsigned char> AESWrapper::generateKey() {
    std::vector<unsigned char> key(32); // 256비트 AES 키 (32 바이트)
    if (!RAND_bytes(key.data(), static_cast<int>(key.size()))) { // size_t에서 int로 변환
        handleCryptoErrors();
    }
    return key;
}

std::vector<unsigned char> AESWrapper::generateIV() {
    std::vector<unsigned char> iv(16); // 128비트 IV (16 바이트)
    if (!RAND_bytes(iv.data(), static_cast<int>(iv.size()))) { // size_t에서 int로 변환
        handleCryptoErrors();
    }
    return iv;
}

std::vector<unsigned char> AESWrapper::encryptWithAES(const std::vector<unsigned char>& plaintext,
    const std::vector<unsigned char>& key,
    const std::vector<unsigned char>& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        handleCryptoErrors();
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), nullptr, key.data(), iv.data())) {
        handleCryptoErrors();
    }

    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_ctr()));
    int len;
    int ciphertext_len;

    if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), static_cast<int>(plaintext.size()))) { // size_t에서 int로 변환
        handleCryptoErrors();
    }
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) {
        handleCryptoErrors();
    }
    ciphertext_len += len;
    ciphertext.resize(ciphertext_len);

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext;
}

std::vector<unsigned char> AESWrapper::decryptWithAES(const std::vector<unsigned char>& ciphertext,
    const std::vector<unsigned char>& key,
    const std::vector<unsigned char>& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        handleCryptoErrors();
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), nullptr, key.data(), iv.data())) {
        handleCryptoErrors();
    }

    std::vector<unsigned char> plaintext(ciphertext.size());
    int len;
    int plaintext_len;

    if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), static_cast<int>(ciphertext.size()))) { // size_t에서 int로 변환
        handleCryptoErrors();
    }
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) {
        handleCryptoErrors();
    }
    plaintext_len += len;
    plaintext.resize(plaintext_len);

    EVP_CIPHER_CTX_free(ctx);

    return plaintext;
}

void AESWrapper::handleCryptoErrors() const {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("OpenSSL encountered an error");
}
