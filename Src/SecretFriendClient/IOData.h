#pragma comment(lib, "ws2_32")

#ifndef IODATA_H
#define IODATA_H

#include <winsock2.h>
#include <iostream>
#include <array>

constexpr int SOCKET_BUFFER_SIZE = 1024;

enum IOOperation
{
	IO_RECV = 0,
	IO_SEND = 1,
	IO_ERROR
};

struct IO_DATA
{
private:
	OVERLAPPED		overlapped_;
	IOOperation		ioType_;
	size_t totalBytes_;
	size_t currentBytes_;

public:
	std::array<char, SOCKET_BUFFER_SIZE> buffer_;
	IO_DATA(IOOperation type) : ioType_(type)
	{
		ZeroMemory(&overlapped_, sizeof(overlapped_));
		this->clear();
	}

	~IO_DATA() {}

	IOOperation& Type() { return ioType_; }

	void clear()
	{
		buffer_.fill(0);
		totalBytes_ = 0;
		currentBytes_ = 0;
	}


	WSABUF SetData(BYTE pType, PBYTE pData, WORD dataSize)
	{
		size_t sendSize = dataSize;
		buffer_.at(0) = 0x24; buffer_.at(1) = 0x08; buffer_.at(2) = 0x01; buffer_.at(3) = 0x05;
		memcpy_s(buffer_.data() + 4, sizeof(WORD), &dataSize, sizeof(WORD));
		buffer_.at(6) = pType;
		memcpy_s(buffer_.data() + 7, buffer_.max_size() - 7, pData, dataSize);

		WSABUF wsabuf;
		wsabuf.buf = buffer_.data();
		wsabuf.len = sendSize;

		return wsabuf;
	}

	LPWSAOVERLAPPED overlapped() { return &overlapped_; }

	void setType(IOOperation type) { ioType_ = type; }

	char* Data() { return buffer_.data(); }

	WSABUF WsaBuf()
	{
		WSABUF wsabuf;
		wsabuf.buf = buffer_.data();
		wsabuf.len = SOCKET_BUFFER_SIZE;

		return wsabuf;
	}

	WSABUF SendSet(int dataSize, char* pMsg)
	{
		ZeroMemory(this, sizeof(IO_DATA));
		ioType_ = IO_SEND;
		WSABUF wsabuf;
		wsabuf.len = dataSize;
		wsabuf.buf = buffer_.data();
		std::copy_n(pMsg, dataSize, wsabuf.buf);

		return wsabuf;
	}
};

#endif