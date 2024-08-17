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

	// 5. 패킷 전체를 포함할 데이터 배열 생성
	std::vector<BYTE> packetData;
	packetData.reserve(HDR_FOOTER_SIZE + HDR_SIZE_DEF + dataSize); // 전체 크기 예약

	// 6. 패킷 데이터 구성
	packetData.insert(packetData.end(), footer.begin(), footer.end());       // 패킷 푸터 추가
	packetData.insert(packetData.end(), dataSizeArr.begin(), dataSizeArr.end()); // 데이터 크기 추가
	packetData.push_back(packetType);                                        // PacketType 추가
	packetData.insert(packetData.end(), publickey.begin(), publickey.end()); // 대칭 키 데이터 추가

	// 7. 패킷 전송
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

	// 5. 패킷 전체를 포함할 데이터 배열 생성
	std::vector<BYTE> packetData;
	packetData.reserve(HDR_FOOTER_SIZE + HDR_SIZE_DEF + dataSize); // 전체 크기 예약

	// 6. 패킷 데이터 구성
	packetData.insert(packetData.end(), footer.begin(), footer.end());       // 패킷 푸터 추가
	packetData.insert(packetData.end(), dataSizeArr.begin(), dataSizeArr.end()); // 데이터 크기 추가
	packetData.push_back(packetType);                                        // PacketType 추가
	packetData.insert(packetData.end(), data.begin(), data.end()); // 대칭 키 데이터 추가

	// 7. 패킷 전송
	return requestorSession->SendPacket(packetData);
	return true;
}