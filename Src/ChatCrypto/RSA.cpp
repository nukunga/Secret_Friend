#include "pch.h"
#include "RSA.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <stdexcept>

RSAEncryption::RSAEncryption() {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) {
        handleCryptoErrors();
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleCryptoErrors();
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 4096) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleCryptoErrors();
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleCryptoErrors();
    }

    EVP_PKEY_CTX_free(ctx);
}

RSAEncryption::~RSAEncryption() {
    if (pkey) {
        EVP_PKEY_free(pkey);
    }
}

std::string RSAEncryption::exportPublicKeyPEM() const {
    BIO* publicBIO = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PUBKEY(publicBIO, pkey)) {
        handleCryptoErrors();
    }

    char* publicKeyPEM;
    long publicKeyLen = BIO_get_mem_data(publicBIO, &publicKeyPEM);

    std::string publicKeyStr(publicKeyPEM, publicKeyLen);
    BIO_free(publicBIO);

    return publicKeyStr;
}

std::string RSAEncryption::exportPrivateKeyPEM() const {
    BIO* privateBIO = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PrivateKey(privateBIO, pkey, nullptr, nullptr, 0, nullptr, nullptr)) {
        handleCryptoErrors();
    }

    char* privateKeyPEM;
    long privateKeyLen = BIO_get_mem_data(privateBIO, &privateKeyPEM);

    std::string privateKeyStr(privateKeyPEM, privateKeyLen);
    BIO_free(privateBIO);

    return privateKeyStr;
}

std::vector<unsigned char> RSAEncryption::encryptWithPublicKey(const std::vector<unsigned char>& plaintext) const {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) {
        handleCryptoErrors();
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleCryptoErrors();
    }

    size_t outlen;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outlen, plaintext.data(), plaintext.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleCryptoErrors();
    }

    std::vector<unsigned char> ciphertext(outlen);
    if (EVP_PKEY_encrypt(ctx, ciphertext.data(), &outlen, plaintext.data(), plaintext.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleCryptoErrors();
    }

    EVP_PKEY_CTX_free(ctx);
    return ciphertext;
}

std::vector<unsigned char> RSAEncryption::decryptWithPrivateKey(const std::vector<unsigned char>& ciphertext) const {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) {
        handleCryptoErrors();
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleCryptoErrors();
    }

    size_t outlen;
    if (EVP_PKEY_decrypt(ctx, nullptr, &outlen, ciphertext.data(), ciphertext.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleCryptoErrors();
    }

    std::vector<unsigned char> plaintext(outlen);
    if (EVP_PKEY_decrypt(ctx, plaintext.data(), &outlen, ciphertext.data(), ciphertext.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleCryptoErrors();
    }

    EVP_PKEY_CTX_free(ctx);
    return plaintext;
}

void RSAEncryption::handleCryptoErrors() const {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("OpenSSL encountered an error");
}
