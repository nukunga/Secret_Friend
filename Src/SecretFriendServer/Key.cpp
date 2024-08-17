#include "Key.h"

bool KeyManager::ReceivePublicKey(Session* requestorSession, const std::array<BYTE, RSA_KEY_SIZE>& data)
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

bool KeyManager::ReceiveSymmetricKey(Session* requestorSession, const std::array<BYTE, AES_KEY_SIZE>& data)
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

	std::array<BYTE, HDR_FOOTER_SIZE> footer = { 0x24, 0x08, 0x01, 0x05 };

	WORD dataSize = RSA_KEY_SIZE / 8 + 1;
	std::array<BYTE, HDR_SIZE_DEF> dataSizeArr = { static_cast<BYTE>(dataSize & 0xFF), static_cast<BYTE>((dataSize >> 8) & 0xFF) };

	BYTE packetType = static_cast<BYTE>(PacketType::SERVER_SEND_OPPONENT_PUBLIC_KEY);

	std::array<BYTE, RSA_KEY_SIZE> publickey = requestorSession->GetPublicKey();

	// 5. ��Ŷ ��ü�� ������ ������ �迭 ����
	std::vector<BYTE> packetData;
	packetData.reserve(HDR_FOOTER_SIZE + HDR_SIZE_DEF + dataSize); // ��ü ũ�� ����

	// 6. ��Ŷ ������ ����
	packetData.insert(packetData.end(), footer.begin(), footer.end());       // ��Ŷ Ǫ�� �߰�
	packetData.insert(packetData.end(), dataSizeArr.begin(), dataSizeArr.end()); // ������ ũ�� �߰�
	packetData.push_back(packetType);                                        // PacketType �߰�
	packetData.insert(packetData.end(), publickey.begin(), publickey.end()); // ��Ī Ű ������ �߰�

	// 7. ��Ŷ ����
	return requestorSession->SendPacket(packetData);
	return true;
}



bool KeyManager::ReceiveRoomSymmetricKey(Session* requestorSession, const std::array<BYTE, RSA_KEY_SIZE>& data)
{
	return SendRoomSymmetricKey(requestorSession, data);
}


bool KeyManager::SendRoomSymmetricKey(Session* requestorSession, const std::array<BYTE, RSA_KEY_SIZE>& data)
{
	std::array<BYTE, HDR_FOOTER_SIZE> footer = { 0x24, 0x08, 0x01, 0x05 };

	WORD dataSize = RSA_KEY_SIZE / 8 + 1;
	std::array<BYTE, HDR_SIZE_DEF> dataSizeArr = { static_cast<BYTE>(dataSize & 0xFF), static_cast<BYTE>((dataSize >> 8) & 0xFF) };

	BYTE packetType = static_cast<BYTE>(PacketType::SERVER_SEND_ROOM_SYMMETRICKEY);

	std::array<BYTE, RSA_KEY_SIZE> publickey = requestorSession->GetPublicKey();

	// 5. ��Ŷ ��ü�� ������ ������ �迭 ����
	std::vector<BYTE> packetData;
	packetData.reserve(HDR_FOOTER_SIZE + HDR_SIZE_DEF + dataSize); // ��ü ũ�� ����

	// 6. ��Ŷ ������ ����
	packetData.insert(packetData.end(), footer.begin(), footer.end());       // ��Ŷ Ǫ�� �߰�
	packetData.insert(packetData.end(), dataSizeArr.begin(), dataSizeArr.end()); // ������ ũ�� �߰�
	packetData.push_back(packetType);                                        // PacketType �߰�
	packetData.insert(packetData.end(), data.begin(), data.end()); // ��Ī Ű ������ �߰�

	// 7. ��Ŷ ����
	return requestorSession->SendPacket(packetData);
	return true;
}