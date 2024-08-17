#include "RSA.h"
#include "AES.h"
#include <iostream>
#include <vector>
#include <string>

void testRSAEncryption() {
    try {
        // RSA 키 생성
        RSAEncryption rsa(2048);

        // 공개키 및 개인키 PEM 형식으로 출력
        std::string publicKeyPEM = rsa.exportPublicKeyPEM();
        std::string privateKeyPEM = rsa.exportPrivateKeyPEM();
        
        std::cout << "RSA Public Key PEM:\n" << publicKeyPEM << std::endl;
        std::cout << "RSA Private Key PEM:\n" << privateKeyPEM << std::endl;

        // 암호화할 메시지
        std::string message = "This is a test message for RSA encryption!";
        std::vector<unsigned char> plaintext(message.begin(), message.end());

        // RSA 암호화
        std::vector<unsigned char> ciphertext = rsa.encryptWithPublicKey(plaintext);

        std::cout << "RSA Encrypted Message (in hex): ";
        for (unsigned char c : ciphertext) {
            std::cout << std::hex << (int)c;
        }
        std::cout << std::endl;

        // RSA 복호화
        std::vector<unsigned char> decryptedtext = rsa.decryptWithPrivateKey(ciphertext);

        std::string decryptedMessage(decryptedtext.begin(), decryptedtext.end());
        std::cout << "RSA Decrypted Message: " << decryptedMessage << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "RSA Encryption/Decryption Error: " << e.what() << std::endl;
    }
}

void testAESEncryption() {
    try {
        // AESWrapper 인스턴스 생성
        AESEncryption aes;

        // AES 키 및 IV 생성
        std::vector<unsigned char> key = aes.generateAESKey();
        std::vector<unsigned char> iv = aes.generateAESIV();

        // 암호화할 메시지
        std::string message = "This is a test message for AES encryption!";
        std::vector<unsigned char> plaintext(message.begin(), message.end());

        // AES 암호화
        std::vector<unsigned char> ciphertext = aes.encryptWithAES(plaintext, key, iv);

        std::cout << "AES Encrypted Message (in hex): ";
        for (unsigned char c : ciphertext) {
            std::cout << std::hex << (int)c;
        }
        std::cout << std::endl;

        // AES 복호화
        std::vector<unsigned char> decryptedtext = aes.decryptWithAES(ciphertext, key, iv);

        std::string decryptedMessage(decryptedtext.begin(), decryptedtext.end());
        std::cout << "AES Decrypted Message: " << decryptedMessage << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "AES Encryption/Decryption Error: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "Testing RSA Encryption/Decryption:" << std::endl;
    testRSAEncryption();

    std::cout << "\nTesting AES Encryption/Decryption:" << std::endl;
    testAESEncryption();

    return 0;
}
