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
	// 맵 


private:
	HANDLE								hIOCP;
	WSADATA								WSAData;
	SOCKADDR_IN							serverAddr;
	SOCKET								listenSocket;

	std::vector<std::thread>			workerThreads;
	std::priority_queue<TimerEvent, std::vector<TimerEvent>, TimerEvent::Priority>	timerQueue;
	std::mutex					timerQueue_Lock;
	SOCKETINFO							objects[MAX_USER];	// client, 투사체, 큐브 장애물 등에 다 ID를 등록 후 클라이언트에 뿌려줘야 하므로 Objects라고 명명
	float fDistance = 10.0f;

	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3     				m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float           			m_fMaxVelocityXZ = 0.0f;
	float           			m_fMaxVelocityY = 0.0f;
	float           			m_fFriction = 0.0f;

	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

private:
	bool NetworkInitialize();

	static DWORD WINAPI RegisterAcceptThread(LPVOID self);
	void AcceptThread();
	static DWORD WINAPI RegisterWorkerThread(LPVOID self);
	void WorkerThread();
	static DWORD WINAPI RegisterTimerThread(LPVOID self);
	void TimerThread();

	void ObjectInitialize();
	void SendPacket(unsigned short int id, char *packet);	// packet을 id 클라이언트에 보낸다
	void RecvPacket(unsigned short int id);	// 해당 id의 클라이언트의 패킷을 받아온다
	void SendLoginOKPacket(unsigned short int to);
	void SendPutPlayerPacket(unsigned short int to, unsigned short int obj);
	void SendPositionPacket(unsigned short to, unsigned short obj);
	void SendRemovePlayerPacket(unsigned short int to, unsigned short int id);
	void ProcessPacket(unsigned short int id, char *buf);
	void SendMapInfoPacket(unsigned short to, unsigned short obj);
	void DisConnect(unsigned short int id);
	const unsigned short int GetNewID();
	bool isNearObject(unsigned short int a, unsigned short int b);
	float Distance(XMFLOAT3 vector1, XMFLOAT3 vector2);

	void Move(const XMFLOAT3 & xmf3Shift, bool bUpdateVelocity);
	void Update(float fTimeElapsed);


	void Collision(unsigned int id);
	void AddTimerEvent(unsigned int id, TimerEvent::Command cmd = TimerEvent::Command::Collision, double seconds = 1.0);
};