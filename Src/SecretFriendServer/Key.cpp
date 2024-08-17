#include "Key.h"

bool KeyManager::ReceivePublicKey(Session* requestorSession, const std::vector<BYTE>& data)
{
	if (data.empty())
	{
		printf("[ERROR] Received empty public key data.\n");
		return false;
	}

	mtx.lock();
	requestorSession->StorePublicKey(data);
	mtx.unlock();
	//printf("[INFO] Public key received and stored successfully.\n");
	return true;
}

bool KeyManager::ReceiveSymmetricKey(Session* requestorSession, const std::vector<BYTE>& data)
{
	if (data.size() != AES_KEY_SIZE / 8)
	{
		printf("[ERROR] Invalid symmetric key size received: %zu bytes\n", data.size());
		return false;
	}

	mtx.lock();
	requestorSession->StoreAESKey(data);
	mtx.unlock();

	//printf("[INFO] Symmetric key received successfully.\n");
	return true;
}

bool KeyManager::SendGuestPublicKey(Session* requestorSession)
{
	std::vector<BYTE> publickey = requestorSession->GetPublicKey();

	// 7. 패킷 전송
	return requestorSession->SendPacket(publickey, SERVER_SEND_OPPONENT_PUBLIC_KEY);
}



bool KeyManager::ReceiveRoomSymmetricKey(Session* requestorSession, const std::vector<BYTE>& data)
{
	return SendRoomSymmetricKey(requestorSession, data);
}


bool KeyManager::SendRoomSymmetricKey(Session* requestorSession, const std::vector<BYTE>& data)
{
	// 7. 패킷 전송
	return requestorSession->SendPacket(data, SERVER_SEND_ROOM_SYMMETRICKEY);
	return true;
}