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
		objects[new_id].over.is_recv = true;
		std::random_device rd;
		std::default_random_engine dre(rd());
		std::uniform_real_distribution<> startPos(0.0, 100.0);

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
			if (true == isNearObject(i, new_id)) {
				objects[i].viewlist.insert(new_id);
				SendPutPlayerPacket(i, new_id);
			}
		}
		for (int i = 0; i < MAX_USER; ++i) {
			if (false == objects[i].connected) continue;
			if (i == new_id) continue;
			if (true == isNearObject(i, new_id)) {
				objects[new_id].viewlist.insert(i);
				SendPutPlayerPacket(new_id, i);
			}
		}


		for (int i = Cube_start; i < Cube_start + 50; ++i) {
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

		if (lpover_ex->is_recv) {
			int rest_size = io_byte;
			char *ptr = lpover_ex->messageBuffer;
			char packet_size = 0;
			if (0 < objects[key].prev_size) packet_size = objects[key].packet_buf[0];
			while (rest_size > 0) {
				if (packet_size == 0)
					packet_size = ptr[0];
				int required = packet_size - objects[key].prev_size;
				if (rest_size >= required) {
					memcpy(objects[key].packet_buf + objects[key].prev_size, ptr, required);
					ProcessPacket(key, objects[key].packet_buf);
					rest_size -= required;
					ptr += required;
					packet_size = 0; // 패킷 처리가 끝남
				}
				else {
					// 패킷을 더 이상 만들 수가 없다
					memcpy(objects[key].packet_buf + objects[key].prev_size, ptr, rest_size); // 포인터에 들어있는 내용을 몽땅 다 쏟아붓는다
					rest_size = 0;
				}
			}
			RecvPacket(key);
		}
		else {
			// 오버랩구조체 delete
			delete lpover_ex;
		}
	}
}


void ServerManager::ObjectInitialize()
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	//std::uniform_real_distribution<> startPos(-2650.0,2650.0);
	//std::uniform_real_distribution<> startYPos(0, 2650.0);
	std::uniform_real_distribution<> startRotate(0, 90.0);
	std::uniform_real_distribution<> startPos(0.0, 500.0);
	std::uniform_real_distribution<> startYPos(0, 250.0);

	for (int i = Cube_start; i < Cube_start+5; ++i) {
		objects[i].position.x = startPos(dre);
		objects[i].position.y = startYPos(dre);
		objects[i].position.z = startPos(dre);

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);
		
		objects[i].cube_size = 300;

	}
	for (int i = Cube_start+5; i < Cube_start + 10; ++i) {
		objects[i].position.x = startPos(dre);
		objects[i].position.y = startYPos(dre);
		objects[i].position.z = startPos(dre);

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);

		objects[i].cube_size = 400;
	}
	for (int i = Cube_start+10; i < Cube_start + 20; ++i) {
		objects[i].position.x = startPos(dre);
		objects[i].position.y = startYPos(dre);
		objects[i].position.z = startPos(dre);

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);

		objects[i].cube_size = 500;
	}
	for (int i = Cube_start+20; i < Cube_start + 30; ++i) {
		objects[i].position.x = startPos(dre);
		objects[i].position.y = startYPos(dre);
		objects[i].position.z = startPos(dre);

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);

		objects[i].cube_size = 600;
	}
	for (int i = Cube_start+30; i < Cube_start + 50; ++i) {
		objects[i].position.x = startPos(dre);
		objects[i].position.y = startYPos(dre);
		objects[i].position.z = startPos(dre);

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);

		objects[i].cube_size = 700;
	}


	/*for (auto &cl : objects) {
		cl.connected = false;
		cl.viewlist.clear();
	}*/
}

void ServerManager::SendPacket(unsigned short int id, char * packet)
{
	SOCKET client_s = objects[id].socket;

	OVER_EX *over = reinterpret_cast<OVER_EX *>(malloc(sizeof(OVER_EX)));

	over->dataBuffer.len = packet[0];
	over->dataBuffer.buf = over->messageBuffer;
	memcpy(over->messageBuffer, packet, packet[0]);
	::ZeroMemory(&(over->overlapped), sizeof(WSAOVERLAPPED));
	over->is_recv = false;
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
	packet.cube_size = objects[obj].cube_size;
	packet.position = XMFLOAT3(objects[obj].position.x, objects[obj].position.y, objects[obj].position.z);
	packet.rotate = XMFLOAT3(objects[obj].rotate.x, objects[obj].rotate.y, objects[obj].rotate.z);

	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::ProcessPacket(unsigned short int id, char * buf)
{
	CSPacket_Move * packet = reinterpret_cast<CSPacket_Move *>(buf);

	XMFLOAT3 pos = objects[id].position;
	switch (packet->type) {
	case CS_Type::Move:
	{
		
		if ((DIR_FORWARD & packet->dir) && pos.y < WORLD_HEIGHT - 1) ++pos.y;
		if ((DIR_BACKWARD & packet->dir) && pos.y > 0) --pos.y;
		if ((DIR_LEFT & packet->dir) && pos.x > 0) --pos.x;
		if ((DIR_RIGHT & packet->dir) && pos.x < WORLD_WIDTH - 1) ++pos.x;
	}
	break;
	default:
		serverPrint("Unknown Packet Type Error\n");
		while (true);
	}
	objects[id].position = pos;

	// 시야 처리 부분
	objects[id].vl_lock.lock();
	std::unordered_set < unsigned short int> old_vl = objects[id].viewlist;
	objects[id].vl_lock.unlock();
	std::unordered_set <unsigned short int> new_vl;

	for (int i = 0; i < MAX_USER; ++i) {
		if ((true == objects[i].connected) &&
			(true == isNearObject(id, i)) &&
			(i != id))
			new_vl.insert(i);
	}

	for (auto cl : new_vl)
	{
		if (0 != old_vl.count(cl)) {	// old에 있었다, old, new 동시 존재
			objects[id].vl_lock.lock();
			if (0 != objects[cl].viewlist.count(id)) {// 상대방 뷰리스트에 있나
				objects[id].vl_lock.unlock();
				SendPositionPacket(cl, id);
			}
			else {
				objects[cl].viewlist.insert(id);
				objects[id].vl_lock.unlock();
				SendPutPlayerPacket(cl, id);	// 없었으면 추가
			}
		}
		else {	// 새로 시야에 들어옴.
			objects[id].vl_lock.lock();
			objects[id].viewlist.insert(cl);
			objects[id].vl_lock.unlock();
			SendPutPlayerPacket(id, cl);
			objects[id].vl_lock.lock();
			if (0 != objects[cl].viewlist.count(id)) {	// 상대방이 이미 나를 알고 있는가?
				objects[id].vl_lock.unlock();
				SendPositionPacket(cl, id);
			}
			else {	// 아니라면 추가해야지.
				objects[cl].viewlist.insert(id);
				objects[id].vl_lock.unlock();
				SendPutPlayerPacket(cl, id);
			}
		}
	}
	for (auto cl : old_vl) { 	// 시야에서 사라짐
		if (0 != new_vl.count(cl)) continue;	// 이미 위에서 처리를 했으니 할 게 없다. 컨티뉴
		objects[id].vl_lock.lock();
		objects[id].viewlist.erase(cl);
		objects[id].vl_lock.unlock();
		SendRemovePlayerPacket(id, cl);
		objects[id].vl_lock.lock();
		if (0 != objects[cl].viewlist.count(id)) // 상대방 뷰 리스트에 내가 없는 경우에만!
		{
			objects[cl].viewlist.erase(id);	// 있나 없나 보고 erase해주어야 한다. 그냥 하면 안된다.
			objects[id].vl_lock.unlock();
			SendRemovePlayerPacket(cl, id);
		}
		else
			objects[id].vl_lock.unlock();
	}
	SendPositionPacket(id, id);	// 나한테 보내준다

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
