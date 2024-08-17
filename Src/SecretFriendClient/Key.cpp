#include "Key.h"

void Key::StorePublicKey(const std::vector<BYTE>& publicKey)
{
	std::copy(publicKey.begin(), publicKey.end(), PublicKey.begin());
}

void Key::StoreAESKey(const std::vector<BYTE>& aesKey)
{
	std::copy(aesKey.begin(), aesKey.end(), AESKey.begin());
}

std::vector<BYTE> Key::GetPublicKey() const
{
	return PublicKey;
}
