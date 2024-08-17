#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

#include "IOData.h"
#include "Key.h"
#include <queue>
#include <mutex>

constexpr int HDR_FOOTER_SIZE = 4;
constexpr int HDR_SIZE_DEF = 2;

/*
*			BoBChat 패킷 구조
* 
* ==== 이 부분은 암호화되지 않음 ====
* 0x00~0x03 (4B) = 패킷 푸터 (0x24, 0x08, 0x01, 0x05)
* 0x04~0x05 (2B) = 패킷 데이터 크기
* ==== 이 부분은 암호화되지 않음 ====
* 
* 
* 
* ==== 이 부분은 서버<->클라이언트 키로 암호화 되어야함 ====
* 0x06 (1B) = PacketType (패킷 종류)
* ~... = 패킷 종류별로 상이한 데이터
* ==== 이 부분은 서버<->클라이언트 키로 암호화 되어야함 ====
* ex) 예를들어 상대와의 채팅은 별개의 키로 암호화가 된다. 해당 통신 또한 서버<->클라이언트 키로 암호화가 된다.
* ex) 따라서 상대방은 이를 수신시 서버<->클라이언트 키로 복호화 후 채팅방에서 교환한 키로 복호화, 총 2번 복호화를 한다.
*/

enum PacketType
{
	E_PK_UNDEFINED = 0,

	/* From Client */
	CLIENT_SEND_PUBLICKEY = 100, // CLIENT의 공개키 전송
	CLIENT_SEND_SYMMETRICKEY = 101, // CLIENT의 대칭키 전송
	CLIENT_SEND_SESSIONID = 102, // SESSIOINID 전송
	
	CLIENT_REQ_ROOM_LIST = 110, // ROOM LIST 요청
	CLIENT_REQ_ROOM_CREATE = 111, // ROOM 생성 요청
	CLIENT_REQ_ROOM_ENTER = 112, // ROOM 입장 요청
	CLIENT_REQ_ROOM_EXIT = 113, // ROOM 퇴장 요청
	CLIENT_REQ_OPPONENT_PUBLIC_KEY = 114, // 상대방의 공개키 요청

	CLIENT_SEND_CHAT = 120, // 메세지 전송

	/* From Server */
	SERVER_REQ_SUCCESS = 200, // 성공 응답 
	SERVER_REQ_FAIL = 201, // 실패 응답
	SERVER_PUBLIC_KEY = 202, // SERVER의 공개키

	SERVER_ROOM_LIST = 210, // SERVER의 ROOM LIST
	SERVER_ROOM_INFO = 211, // SERVER의 ROOM 정보
	SERVER_SEND_OPPONENT_PUBLIC_KEY = 212, // SERVER에서 Guest의 공개키를 전송
	SERVER_SEND_ROOM_SYMMETRICKEY = 213,

	SERVER_SEND_CHAT = 220, // SERVER에서의 메세지 전송
	SERVER_JOIN_NOTIFY = 221,
	SERVER_LEAVE_NOTIFY = 222,
	SERVER_ROOM_DESTROY_NOTIFY = 223,

};

class PacketBuilder
{
public:
	PacketBuilder()
	{
		Hdr.fill(0);
		Data.fill(0);
		DataSize = 0;
		PType = E_PK_UNDEFINED;
	}

	~PacketBuilder()
	{}

	void PushRecvPacket(IO_DATA ioData, size_t ioSize);
	void ValidatePacket();
	void ParsePacket();
	void InitializeReceiver();

private:
	bool ValidateHeader();

	std::queue<BYTE> PacketQueue;			// 패킷 큐에 저장 후 순서대로 꺼내서 처리
	std::array<BYTE, HDR_FOOTER_SIZE> Hdr;	// 현재 패킷 객체의 헤더 유효성 검사용 시그니쳐 0x24, 0x08, 0x01, 0x05
	std::array<BYTE, SOCKET_BUFFER_SIZE> Data;	// 현재 파싱이 끝난 패킷
	WORD DataSize = 0;						// 현재 패킷 객체에서 수용할 패킷 크기
	PacketType PType;						// 패킷 형태 구분
	std::mutex Mtx;							// Mutual exclusion 방지를 위한 뮤텍스 (세션별로 생성된다)
};

#endif