#include "stdafx.h"
#include "ServerManager.h"

ServerManager::ServerManager()
{
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);	// IOCP 객체 생성
	if (!NetworkInitialize()) { 
		serverPrint("NetworkInitialize Error\n"); 
		ServerManager::~ServerManager();
		if(this) delete this;
	}
}


ServerManager::~ServerManager()
{
	workerThreads.clear();
	closesocket(listenSocket);
	WSACleanup();
	CloseHandle(hIOCP);
}

void ServerManager::Run()
{
	ObjectInitialize();
	_wsetlocale(LC_ALL, L"korean");
	std::wcout.imbue(std::locale("korean"));

	workerThreads.reserve(4);
	for (int i = 0; i < 4; ++i)
		workerThreads.emplace_back(std::thread{ RegisterWorkerThread, (LPVOID)this });
	std::thread accept_thread{ RegisterAcceptThread, (LPVOID)this };
	std::thread timerThread{ RegisterTimerThread, (LPVOID)this };

	timerThread.join();

	accept_thread.join();
	for (auto & thread : workerThreads) thread.join();
}

bool ServerManager::NetworkInitialize()
{
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		serverPrint("Error - Can't load 'winsock.dll' file\n");
		return false;
	}

	// 1. 소켓생성  
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET) {
		serverPrint("Error - Invalid socket\n");
		return false;
	}

	// 서버정보 객체설정
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = PF_INET; // AF_INET;
	serverAddr.sin_port = htons(SERVERPORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 2. 소켓설정
	// bind가 기본 C++ 키워드가 되었으므로 ::을 붙여서 함수임을 표기한다.
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		serverPrint("Error - Fail bind\n");
		return false;
	}

	// 3. 수신대기열생성
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		serverPrint("Error - Fail listen\n");
		return false;
	}

	serverPrint("Server Start\n");
	return true;
}

DWORD WINAPI ServerManager::RegisterAcceptThread(LPVOID self)
{
	ServerManager* mySelf = static_cast<ServerManager*>(self);
	mySelf->AcceptThread();

	return 0;
}

void ServerManager::AcceptThread()
{
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;
	DWORD flags;

	while (true) {
		clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET) {
			serverPrint("Error - Accept Failure\n");
			return;
			// continue;
		}

		char new_id = GetNewID();

		// 클라이언트 구조체 초기화
		// memset(&clients[new_id], 0x00, sizeof(struct SOCKETINFO)); // viewlist같은 컨테이너객체는 0으로 초기화해서는 안된다.
		objects[new_id].socket = clientSocket;
		objects[new_id].over.dataBuffer.len = BUFSIZE;
		objects[new_id].over.dataBuffer.buf = objects[clientSocket].over.messageBuffer;
		objects[new_id].over.type = OVER_EX::Type::RECV;
		std::random_device rd;
		std::default_random_engine dre(rd());
		std::uniform_real_distribution<> startPos(0.0, 0.0);

		objects[new_id].position = { static_cast<float>(startPos(dre)), static_cast<float>(startPos(dre)), static_cast<float>(startPos(dre)) };
		objects[new_id].viewlist.clear();
		objects[new_id].prev_size = 0;
		ZeroMemory(&objects[new_id].over.overlapped, sizeof(WSAOVERLAPPED));	// 초기화 안하면 제대로 오버랩I/O가 동작 안함
		flags = 0;

		// IOCP 연결
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), hIOCP, new_id, 0);

		objects[new_id].connected = true;

		serverPrint("Client [%d] Connected\n", new_id);
		
		SendLoginOKPacket(new_id);
		SendPutPlayerPacket(new_id, new_id); // 나 자신에게 미리 알려준다.
		for (int i = 0; i < MAX_USER; ++i) {
			if (false == objects[i].connected) continue;
			if (i == new_id) continue; // 나 자신에게 나를 알려줄 필요는 없다.
			else {
			//	objects[i].viewlist.insert(new_id);
				//AddTimerEvent(new_id);
				SendPutPlayerPacket(i, new_id);
			}
		}
		for (int i = 0; i < MAX_USER; ++i) {
			if (false == objects[i].connected) continue;
			if (i == new_id) continue;
			else {
				//objects[new_id].viewlist.insert(i);
				
				SendPutPlayerPacket(new_id, i);
			}
		}
		
		for (int i = Cube_start; i < Cube_start + 50; ++i)
		{
			//AddTimerEvent(i);
			SendMapInfoPacket(new_id, i);
		}

		
		RecvPacket(new_id);
	}
}

DWORD WINAPI ServerManager::RegisterWorkerThread(LPVOID self)
{
	ServerManager* mySelf = static_cast<ServerManager*>(self);
	mySelf->WorkerThread();

	return 0;
}

void ServerManager::WorkerThread()
{
	
	while (true) {
		DWORD io_byte;
		ULONG key;
		OVER_EX *lpover_ex;

#ifdef _WIN64	// x64 환경인 경우
		bool is_error = GetQueuedCompletionStatus(hIOCP, &io_byte, reinterpret_cast<PULONG_PTR>(&key), reinterpret_cast<LPWSAOVERLAPPED *>(&lpover_ex), INFINITE);
#else
		bool is_error = GetQueuedCompletionStatus(hIOCP, &io_byte, &key, reinterpret_cast<LPWSAOVERLAPPED *>(&lpover_ex), INFINITE);
#endif

		if (false == is_error) serverPrint("GetQueueCompletionStatus ERROR", WSAGetLastError());
		if (0 == io_byte) DisConnect(key);

		switch (lpover_ex->type) {
		case OVER_EX::Type::NONE:
			delete lpover_ex;
			break;
		case OVER_EX::Type::RECV:
		{
			int rest_size = io_byte;
			char *ptr = lpover_ex->messageBuffer;
			char packetSize = 0;
			if (0 < objects[key].prev_size) packetSize = objects[key].packet_buf[0];
			while (rest_size > 0) {
				if (packetSize == 0)
					packetSize = ptr[0];
				int required = packetSize - objects[key].prev_size;
				if (rest_size >= required) {
					memcpy(objects[key].packet_buf + objects[key].prev_size, ptr, required);
					ProcessPacket(key, objects[key].packet_buf);
					rest_size -= required;
					ptr += required;
					packetSize = 0; // 패킷 처리가 끝남
				}
				else {
					// 패킷을 더 이상 만들 수가 없다
					memcpy(objects[key].packet_buf + objects[key].prev_size, ptr, rest_size); // 포인터에 들어있는 내용을 몽땅 다 쏟아붓는다
					rest_size = 0;
				}
			}
			RecvPacket(key);
		}
		break;
		case OVER_EX::Type::EVENT:
		{
			TimerEvent* pEvent = reinterpret_cast<TimerEvent*>(lpover_ex->messageBuffer);
			if (pEvent->command == TimerEvent::Command::Collision)
				Collision(key);
			delete lpover_ex;
		}
		break;
		default:
			serverPrint("이런 패킷 타입을 만든 적이 없는데요...?\n");
			// 오버랩구조체 delete
			delete lpover_ex;
			break;
		}
	}
}

DWORD ServerManager::RegisterTimerThread(LPVOID self)
{
	ServerManager* mySelf = static_cast<ServerManager*>(self);
	mySelf->TimerThread();

	return 0;
}

void ServerManager::TimerThread()
{
	do {
		Sleep(1); // 1ms 강제로 쉬기
		do {
			timerQueue_Lock.lock();
			if (!timerQueue.empty()) {
				TimerEvent peek = timerQueue.top();
				timerQueue_Lock.unlock();
				std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
				if (peek.startTime > currentTime.count()) break;
				//serverPrint("peek [%d] : start time : %.2lf, now : %.2lf\n" , peek.objectID, peek.startTime, currentTime.count());
				timerQueue_Lock.lock();
				timerQueue.pop();
				timerQueue_Lock.unlock();

				OVER_EX* lpover_ex = new OVER_EX;
				memcpy(lpover_ex->messageBuffer, &peek, sizeof(TimerEvent));
				lpover_ex->init();
				lpover_ex->dataBuffer.len = sizeof(TimerEvent);
				lpover_ex->type = OVER_EX::Type::EVENT;

				PostQueuedCompletionStatus(hIOCP, 0, peek.objectID, reinterpret_cast<WSAOVERLAPPED*>(lpover_ex));
			}
			else
				timerQueue_Lock.unlock();
		} while (true);
	} while (true);
}


void ServerManager::ObjectInitialize()
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<> startPos(-2500.0, 2500.0);
	std::uniform_real_distribution<> startYPos(0, 5500.0);
	std::uniform_real_distribution<> startRotate(0, 90.0);

	for (int i = Cube_start; i < Cube_start + 5; ++i) {

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);
	}
	for (int i = Cube_start + 5; i < Cube_start + 15; ++i) {

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);
	}
	for (int i = Cube_start + 15; i < Cube_start + 35; ++i) {

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);
	}
	for (int i = Cube_start + 35; i < Cube_start + 45; ++i) {

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);
	}
	for (int i = Cube_start + 45; i < Cube_start + 50; ++i) {

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);
	}

	// 초기 위치를 설정

	for (int i = Cube_start; i < Cube_start + 50; ++i)
	{
		objects[i].position.x = startPos(dre);
		objects[i].position.y = startYPos(dre);
		objects[i].position.z = startPos(dre);

	}
	//int a = 0;
	
	// 초기 설정된 위치가지고 겹치는지 판단
	//for (int i = Cube_start; i < Cube_start + 50; ++i)
	//{

	//	for (int j = i + 1; j < Cube_start + 50; ++j)
	//	{


	//		// 겹치면 딴데로 보냄
	//		while (1) {
	//			if (Distance(objects[i].position, objects[j].position) <= ((objects[i].cube_size + objects[j].cube_size) / 2) + 300) {
	//				objects[i].position.x = startPos(dre);
	//				objects[i].position.y = startYPos(dre);
	//				objects[i].position.z = startPos(dre);
	//				objects[j].cube_stay = true;
	//			}
	//			else
	//				break;
	//		}
	//	
	//		


	//	}

	//}
	//	

	
	
	

	for (auto &cl : objects) {
		cl.connected = false;
		//cl.viewlist.clear();
	}
}

void ServerManager::SendPacket(unsigned short int id, char * packet)
{
	SOCKET client_s = objects[id].socket;

	OVER_EX *over = reinterpret_cast<OVER_EX *>(malloc(sizeof(OVER_EX)));

	over->dataBuffer.len = packet[0];
	over->dataBuffer.buf = over->messageBuffer;
	memcpy(over->messageBuffer, packet, packet[0]);
	::ZeroMemory(&(over->overlapped), sizeof(WSAOVERLAPPED));
	over->type = OVER_EX::Type::NONE;
	if (WSASend(client_s, &over->dataBuffer, 1, NULL, 0, &(over->overlapped), NULL) == SOCKET_ERROR)
		if (WSAGetLastError() != WSA_IO_PENDING)
			serverPrint("Error = Fail WSASend(error_code : %d)\n", WSAGetLastError());
}

void ServerManager::RecvPacket(unsigned short int id)
{
	DWORD flags = 0;

	SOCKET client_s = objects[id].socket;
	OVER_EX *over = &objects[id].over;

	over->dataBuffer.len = BUFSIZE;
	over->dataBuffer.buf = over->messageBuffer;
	memset(&(over->overlapped), 0x00, sizeof(WSAOVERLAPPED));

	if (WSARecv(client_s, &over->dataBuffer, 1, NULL, &flags, &(over->overlapped), NULL) == SOCKET_ERROR)
		if (WSAGetLastError() != WSA_IO_PENDING)
			serverPrint("RECV ERROR - ", WSAGetLastError());
}

void ServerManager::SendLoginOKPacket(unsigned short int to)
{
	SCPacket_LoginOK packet;
	packet.id = to;
	packet.size = sizeof(packet);
	packet.type = SC_Type::LoginOK;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendPutPlayerPacket(unsigned short int to, unsigned short int obj)
{
	SCPacket_PutPlayer packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::PutPlayer;
	packet.position = objects[obj].position;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendPositionPacket(unsigned short to, unsigned short obj)
{
	// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	SCPacket_Position packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::Position;
	packet.position = objects[obj].position;
	SendPacket(to, reinterpret_cast<char *>(&packet));
	
}

void ServerManager::SendRemovePlayerPacket(unsigned short int to, unsigned short int id)
{
	SCPacket_RemovePlayer packet;
	packet.id = id;
	packet.size = sizeof(packet);
	packet.type = SC_Type::RemovePlayer;
	serverPrint("to [%d] Remove [%d] \n", to, id);
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendMapInfoPacket(unsigned short to, unsigned short obj)
{
	SCPacket_MapInfo packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::MapInfo;
	packet.position = XMFLOAT3(objects[obj].position.x, objects[obj].position.y, objects[obj].position.z);
	packet.rotate = XMFLOAT3(objects[obj].rotate.x, objects[obj].rotate.y, objects[obj].rotate.z);

	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::ProcessPacket(unsigned short int id, char * buf)
{
	
	CSPacket_Move * packet = reinterpret_cast<CSPacket_Move *>(buf);
	XMFLOAT3 xmf3Shift = objects[id].position;
	switch (packet->type) {
	case CS_Type::Move:
	{
		if (packet->dir) {
			/*if (packet->dir & DIR_FORWARD) pos.z+=10;
			if (packet->dir & DIR_BACKWARD) pos.z-=10;
			if (packet->dir & DIR_LEFT) pos.x-=10;
			if (packet->dir & DIR_RIGHT) pos.x+=10;
			if (packet->dir & DIR_UP) pos.y+=10;
			if (packet->dir & DIR_DOWN) pos.y-=10;*/
			
			if (packet->dir & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Up, -fDistance);
			if (packet->dir & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Up, +fDistance);
#ifdef _WITH_LEFT_HAND_COORDINATES
			if (packet->dir & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, +fDistance);
			if (packet->dir & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
#else
			if (packet->dir & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Right, +fDistance);
			if (packet->dir & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Right, -fDistance);
#endif
			if (packet->dir & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Look, +fDistance);
			if (packet->dir & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Look, -fDistance);

		}
	}
	break;
	default:
		serverPrint("Unknown Packet Type Error\n");
		while (true);
	}
	objects[id].position = xmf3Shift;

	


	
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (objects[i].connected == true)
			SendPositionPacket(i, id);
	}
	
	// for (int i = 0; i < MAX_USER; ++i) if (true == clients[i].connected) SendPositionPacket(i, id);
}

void ServerManager::DisConnect(unsigned short int id)
{
	for (int i = 0; i < MAX_USER; ++i) {
		if (false == objects[i].connected) continue;
		objects[i].vl_lock.lock();
		if (0 != objects[i].viewlist.count(id)) { // 뷰리스트에 내가 있으면
			objects[i].vl_lock.unlock();
			SendRemovePlayerPacket(i, id);
		}
		else
			objects[i].vl_lock.unlock();
	}
	closesocket(objects[id].socket);
	objects[id].vl_lock.lock();
	objects[id].viewlist.clear();
	objects[id].vl_lock.unlock();
	objects[id].connected = false;
	serverPrint("client [%d] is disconnected.\n", id);
}

const unsigned short int ServerManager::GetNewID()
{
	while (true)
		for (int i = 0; i < MAX_PLAYER; ++i)	// 플레이어에 새로운 ID를 할당
			if (objects[i].connected == false) {
				objects[i].connected = true;
				return i;
			}
}

bool ServerManager::isNearObject(unsigned short int a, unsigned short int b)
{
	// 바운딩 스피어나 구 충돌 검사 공식을 여기에
	return false;
	return true;
}
// 무시
float ServerManager::Distance(XMFLOAT3 vector1, XMFLOAT3 vector2)
{
	float d;
	d = sqrtf((vector1.x - vector2.x)*(vector1.x - vector2.x) + (vector1.y - vector2.y)*(vector1.y - vector2.y) + (vector1.z - vector2.z)*(vector1.z - vector2.z));

	return d;
}


// 무시
void ServerManager::Collision(unsigned int id)
{
	printf("d");
	

	objects[id].colbox.Center = XMFLOAT3(objects[id].position.x, objects[id].position.y - 60, objects[id].position.z - 50);
	for (int i = Cube_start; i < Cube_start + 5; ++i) {

		objects[i].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400);
	}
	for (int i = Cube_start + 5; i < Cube_start + 15; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 300, MAX_CUBE_SIZE - 300, MAX_CUBE_SIZE - 300);
	}
	for (int i = Cube_start + 15; i < Cube_start + 35; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 200, MAX_CUBE_SIZE - 200, MAX_CUBE_SIZE - 200);
	}
	for (int i = Cube_start + 35; i < Cube_start + 45; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 100, MAX_CUBE_SIZE - 100, MAX_CUBE_SIZE - 100);
	}
	for (int i = Cube_start + 45; i < Cube_start + 50; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE, MAX_CUBE_SIZE, MAX_CUBE_SIZE);
	}
	for (int i = Cube_start; i < Cube_start + 50; ++i)
	{
		objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
		objects[i].colbox.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		
		if (objects[i].colbox.Intersects(objects[id].colbox)) {
			printf("충돌함ㅋ");
		}
		SendPositionPacket(i, id);
	
	}
	//AddTimerEvent(id);
}

void ServerManager::AddTimerEvent(unsigned int id, TimerEvent::Command cmd, double second)
{
	std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch(); // 현재 시간을 double 형태로 가져온다.
	timerQueue_Lock.lock();
	timerQueue.push(TimerEvent(id, cmd, currentTime.count() + second));
	timerQueue_Lock.unlock();
}