#pragma once

class ServerManager
{
public:
	ServerManager();
	~ServerManager();

	// 복사 생성자는 삭제되어야 한다.
	ServerManager(const ServerManager&) = delete;
	ServerManager& operator=(const ServerManager&) = delete;

	// Main 함수에서는 이거 하나만 실행하자. 다른 public 함수는 필요 없다.
	void Run();

private:
	HANDLE								hIOCP;
	WSADATA								WSAData;
	SOCKADDR_IN							serverAddr;
	SOCKET								listenSocket;

	std::vector<std::thread>			workerThreads;

	SOCKETINFO							objects[MAX_USER];	// client, 투사체, 큐브 장애물 등에 다 ID를 등록 후 클라이언트에 뿌려줘야 하므로 Objects라고 명명

private:
	bool NetworkInitialize();

	static DWORD WINAPI RegisterAcceptThread(LPVOID self);
	void AcceptThread();
	static DWORD WINAPI RegisterWorkerThread(LPVOID self);
	void WorkerThread();


	void ObjectInitialize();
	void SendPacket(unsigned short int id, char *packet);	// packet을 id 클라이언트에 보낸다
	void RecvPacket(unsigned short int id);	// 해당 id의 클라이언트의 패킷을 받아온다
	void SendLoginOKPacket(unsigned short int to);
	void SendPutPlayerPacket(unsigned short int to, unsigned short int obj);
	void SendPositionPacket(unsigned short int to, unsigned short int obj);
	void SendRemovePlayerPacket(unsigned short int to, unsigned short int id);
	void ProcessPacket(unsigned short int id, char *buf);

	void DisConnect(unsigned short int id);
	const unsigned short int GetNewID();
	bool isNearObject(unsigned short int a, unsigned short int b);
};