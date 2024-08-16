#pragma comment(lib, "ws2_32")

#ifndef IOCP_H
#define IOCP_H

#include <winsock2.h>
#include <vector>
#include <cstring>
#include <list>
#include <thread>
#include "Session.h"

class IOCP
{
public:
	IOCP(void) :
		ServerSock(INVALID_SOCKET), IocpHandle(INVALID_HANDLE_VALUE),
		workerRunning(true), acceptRuning(true)
	{
		GetSystemInfo(&this->sysInfo);
		this->maximumThread = sysInfo.dwNumberOfProcessors;
	}

	~IOCP(void) { WSACleanup(); }

	bool InitializeSocket();
	bool InitializeServer(int port);

	bool CreateThreads();
	bool StopServer();

	void AddNewSession(Session* session);
	void DeleteSession(Session* session);

	unsigned int AcceptThread();
	unsigned int WorkerThread();

protected:
	SOCKET ServerSock;
	HANDLE IocpHandle;
	std::thread acceptThread;
	std::vector<std::thread> workerThreads;

	bool workerRunning;
	bool acceptRuning;
private:
	SYSTEM_INFO sysInfo;
	DWORD maximumThread;
	std::list<Session*> connectedClients;
};

#endif