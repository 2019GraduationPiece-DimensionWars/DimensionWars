#include "stdafx.h"
#include "ServerManager.h"

ServerManager::ServerManager()
{
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);	// IOCP 객체 생성
	if (!NetworkInitialize()) {
		serverPrint("NetworkInitialize Error\n");
		ServerManager::~ServerManager();
		if (this) delete this;
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

		unsigned short new_id = GetNewID();
		// 클라이언트 구조체 초기화
		// memset(&clients[new_id], 0x00, sizeof(struct SOCKETINFO)); // viewlist같은 컨테이너객체는 0으로 초기화해서는 안된다.
		objects[new_id].socket = clientSocket;
		objects[new_id].over.dataBuffer.len = BUFSIZE;
		objects[new_id].over.dataBuffer.buf = objects[clientSocket].over.messageBuffer;
		objects[new_id].over.type = OVER_EX::Type::RECV;
		std::random_device rd;
		std::default_random_engine dre(rd());
		std::uniform_real_distribution<> startPos(0.0, 0.0);
		std::uniform_int_distribution<> cinfo(0, 1);

		objects[new_id].position = { static_cast<float>(startPos(dre)), 0, static_cast<float>(startPos(dre)) };
	//	objects[new_id].character_info = cinfo(dre);
		objects[new_id].viewlist.clear();
		objects[new_id].prev_size = 0;
		ZeroMemory(&objects[new_id].over.overlapped, sizeof(WSAOVERLAPPED));	// 초기화 안하면 제대로 오버랩I/O가 동작 안함
		flags = 0;

		// IOCP 연결
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), hIOCP, new_id, 0);

		objects[new_id].connected = true;

		serverPrint("Client [%d] Connected\n", new_id);

		SendLoginOKPacket(new_id);

		// 풋플레이어는 룸에서 해주자 
		//SendPutPlayerPacket(new_id, new_id); // 나 자신에게 미리 알려준다.

		//for (int i = 0; i < MAX_PLAYER; ++i) {
		//	if (false == objects[i].connected) continue;
		//	if (i == new_id) continue; // 나 자신에게 나를 알려줄 필요는 없다.
		//	else {
		//		//	objects[i].viewlist.insert(new_id);

		//		SendPutPlayerPacket(i, new_id);
		//	}
		//}
		//for (int i = 0; i < MAX_PLAYER; ++i) {
		//	if (false == objects[i].connected) continue;
		//	if (i == new_id) continue;
		//	else {
		//		//objects[new_id].viewlist.insert(i);

		//		SendPutPlayerPacket(new_id, i);
		//	}
		//}

		// 이거는 맵정보니까  룸에서 하자
		/*for (int i = Cube_start; i < Cube_start + 50; ++i)
		{
			SendMapInfoPacket(new_id, i);
		}

		for (int i = Potal_start; i < Potal_end; ++i)
		{
			SendPotalInfoPacket(new_id, i);
		}


		for (int i = Card_start; i < Card_end; ++i)
		{
			objects[i].position = XMFLOAT3(100000, 0, 100000);
			SendCardPaket(new_id, i);

		}

		for (int i = Slash_start; i < Slash_end; ++i)
		{
			objects[i].position = XMFLOAT3(-100000, 0, 100000);
			SendSlashPaket(new_id, i);
		}*/

		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (objects[i].connected == true)
			{
				AddTimerEvent(i);
			}
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

		if (!is_error) serverPrint("GetQueueCompletionStatus ERROR : %d\n", WSAGetLastError());
		if (!io_byte && key < MAX_USER) DisConnect(key);

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
			if (pEvent->command == TimerEvent::Command::Update) {
				Update(key);
			}
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

				PostQueuedCompletionStatus(hIOCP, 1, peek.objectID, reinterpret_cast<WSAOVERLAPPED*>(lpover_ex));
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
	std::uniform_real_distribution<> startYPos(0, 2500.0);
	std::uniform_real_distribution<> startRotate(0, 0.0);

	for (int i = Cube_start; i < Cube_start + 50; ++i) {

		objects[i].rotate.x = startRotate(dre);
		objects[i].rotate.y = startRotate(dre);
		objects[i].rotate.z = startRotate(dre);
	}

	/*for (int i = Cube_start; i < Cube_start + 50; ++i)
	{
	objects[i].position = XMFLOAT3(startPos(dre), startYPos(dre), startPos(dre));
	}

	for (int i = Cube_start; i < Cube_start + 50; ++i)
	{
	for (int j = i + 1; j < Cube_start + 50; ++j)
	{
	if (Distance(objects[i].position, objects[j].position)<400)
	{
	objects[i].position = XMFLOAT3(startPos(dre), startYPos(dre), startPos(dre));
	if(Distance(objects[i].position, objects[j].position)<500)
	{
	objects[i].position = XMFLOAT3(startPos(dre), startYPos(dre), startPos(dre));
	if (Distance(objects[i].position, objects[j].position)<600)
	{
	objects[i].position = XMFLOAT3(startPos(dre), startYPos(dre), startPos(dre));

	if (Distance(objects[i].position, objects[j].position)<700)
	{
	objects[i].position = XMFLOAT3(startPos(dre), startYPos(dre), startPos(dre));

	}
	}
	}
	}
	}
	}*/

	// 초기 위치를 설정
	


	///////1층///////
	objects[Cube_start].position = XMFLOAT3(0, -100000, 0);

	//400
	objects[Cube_start + 5].position = XMFLOAT3(-950 + center_cube_distance, 2000, -950);
	objects[Cube_start + 6].position = XMFLOAT3(950 + center_cube_distance, 2000, 950);
	//500
	objects[Cube_start + 15].position = XMFLOAT3(-1100 + center_cube_distance, 2000, 0);
	objects[Cube_start + 16].position = XMFLOAT3(0 + center_cube_distance, 2000, -1100);
	objects[Cube_start + 17].position = XMFLOAT3(1100 + center_cube_distance, 2000, 0);
	objects[Cube_start + 18].position = XMFLOAT3(0 + center_cube_distance, 2000, 1100);
	//600
	objects[Cube_start + 35].position = XMFLOAT3(-1050 + center_cube_distance, 2000, 1050);
	objects[Cube_start + 36].position = XMFLOAT3(1050 + center_cube_distance, 2000, -1050);


	objects[Cube_start + 45].position = XMFLOAT3(center_cube_distance, 2000, 0);

	///////2층///////
	objects[Cube_start + 1].position = XMFLOAT3(0, -100000, 0);
	//400

	objects[Cube_start + 7].position = XMFLOAT3(-950, 2000, -950 - center_cube_distance);
	objects[Cube_start + 8].position = XMFLOAT3(950, 2000, 950 - center_cube_distance);
	//500
	objects[Cube_start + 19].position = XMFLOAT3(-1100, 2000, 0 - center_cube_distance);
	objects[Cube_start + 20].position = XMFLOAT3(0, 2000, -1100 - center_cube_distance);
	objects[Cube_start + 21].position = XMFLOAT3(1100, 2000, 0 - center_cube_distance);
	objects[Cube_start + 22].position = XMFLOAT3(0, 2000, 1100 - center_cube_distance);
	//600
	objects[Cube_start + 37].position = XMFLOAT3(-1050, 2000, 1050 - center_cube_distance);
	objects[Cube_start + 38].position = XMFLOAT3(1050, 2000, -1050 - center_cube_distance);


	//700
	objects[Cube_start + 46].position = XMFLOAT3(0, 2000, -center_cube_distance);


	///////3층///////
	objects[Cube_start + 2].position = XMFLOAT3(0, -100000, 0);
	//400
	objects[Cube_start + 9].position = XMFLOAT3(-950, 3000, -950);
	objects[Cube_start + 10].position = XMFLOAT3(950, 3000, 950);
	//500
	objects[Cube_start + 23].position = XMFLOAT3(-1100, 3000, 0);
	objects[Cube_start + 24].position = XMFLOAT3(0, 3000, -1100);
	objects[Cube_start + 25].position = XMFLOAT3(1100, 3000, 0);
	objects[Cube_start + 26].position = XMFLOAT3(0, 3000, 1100);
	//600
	objects[Cube_start + 39].position = XMFLOAT3(-1050, 3000, 1050);
	objects[Cube_start + 40].position = XMFLOAT3(1050, 3000, -1050);
	//center_cube_distance
	objects[Cube_start + 47].position = XMFLOAT3(0, 3000, 0);



	///////4층///////
	objects[Cube_start + 3].position = XMFLOAT3(0, -100000, 0);


	objects[Cube_start + 11].position = XMFLOAT3(-950 - center_cube_distance, 4000, -950);
	objects[Cube_start + 12].position = XMFLOAT3(950 - center_cube_distance, 4000, 950);
	//500
	objects[Cube_start + 27].position = XMFLOAT3(-1100 - center_cube_distance, 4000, 0);
	objects[Cube_start + 28].position = XMFLOAT3(0 - center_cube_distance, 4000, -1100);
	objects[Cube_start + 29].position = XMFLOAT3(1100 - center_cube_distance, 4000, 0);
	objects[Cube_start + 30].position = XMFLOAT3(0 - center_cube_distance, 4000, 1100);
	//600
	objects[Cube_start + 41].position = XMFLOAT3(-1050 - center_cube_distance, 4000, 1050);
	objects[Cube_start + 42].position = XMFLOAT3(1050 - center_cube_distance, 4000, -1050);



	objects[Cube_start + 48].position = XMFLOAT3(-center_cube_distance, 4000, 0);


	///////5층///////
	objects[Cube_start + 4].position = XMFLOAT3(0, -100000, 0);


	objects[Cube_start + 13].position = XMFLOAT3(-950 + center_cube_distance, 4000, -950 + center_cube_distance);
	objects[Cube_start + 14].position = XMFLOAT3(950 + center_cube_distance, 4000, 950 + center_cube_distance);
	//500
	objects[Cube_start + 31].position = XMFLOAT3(-1100 + center_cube_distance, 4000, 0 + center_cube_distance);
	objects[Cube_start + 32].position = XMFLOAT3(0 + center_cube_distance, 4000, -1100 + center_cube_distance);
	objects[Cube_start + 33].position = XMFLOAT3(1100 + center_cube_distance, 4000, 0 + center_cube_distance);
	objects[Cube_start + 34].position = XMFLOAT3(0 + center_cube_distance, 4000, 1100 + center_cube_distance);
	//600
	objects[Cube_start + 43].position = XMFLOAT3(-1050 + center_cube_distance, 4000, 1050 + center_cube_distance);
	objects[Cube_start + 44].position = XMFLOAT3(1050 + center_cube_distance, 4000, -1050 + center_cube_distance);


	objects[Cube_start + 49].position = XMFLOAT3(0 + center_cube_distance, 4000, center_cube_distance);


	for (int i = Cube_start; i < Cube_start + 50; ++i)
	{
		for (int j = i + 1; j < Cube_start + 50; ++j)
		{

			if (Distance(objects[i].position, objects[j].position) < 600)
			{
				objects[i].position = XMFLOAT3(0, -100000, 0);

			}
		}
	}

	// 맨아래
	objects[Potal_start].position = XMFLOAT3(0, 50, 0);
	objects[Potal_start + 1].position = XMFLOAT3(1000, 50, 0);
	objects[Potal_start + 2].position = XMFLOAT3(-1000, 50, 0);
	objects[Potal_start + 3].position = XMFLOAT3(0, 50, 1000);
	objects[Potal_start + 4].position = XMFLOAT3(0, 50, -1000);

	//아래
	objects[Potal_start + 5].position = XMFLOAT3(0-200, 2000+400, -center_cube_distance- 200);
	objects[Potal_start + 6].position = XMFLOAT3(-1100 - 200 + center_cube_distance, 2000+300, 0 - 200);
	objects[Potal_start + 7].position = XMFLOAT3(0 - 200 + center_cube_distance, 2000 + 300, -1100 - 200);
	objects[Potal_start + 8].position = XMFLOAT3(1100 - 200 + center_cube_distance, 2000 + 300, 0 - 200);
	objects[Potal_start + 9].position = XMFLOAT3(0 - 200 + center_cube_distance, 2000 + 300, 1100 - 200);

	//중간
	objects[Potal_start + 10].position = XMFLOAT3(0, 3000+400, 0);
	objects[Potal_start + 11].position = XMFLOAT3(-1100, 3000+300, 0);
	objects[Potal_start + 12].position = XMFLOAT3(0, 3000 + 300, -1100);
	objects[Potal_start + 13].position = XMFLOAT3(1100, 3000 + 300, 0);
	objects[Potal_start + 14].position = XMFLOAT3(0, 3000 + 300, 1100);

	//맨위
	objects[Potal_start + 15].position = XMFLOAT3(0 + center_cube_distance, 4000+400, center_cube_distance);
	objects[Potal_start + 16].position = XMFLOAT3(-1100 + center_cube_distance, 4000+400, 0 + center_cube_distance);
	objects[Potal_start + 17].position = XMFLOAT3(0 + center_cube_distance, 4000 + 400, -1100 + center_cube_distance);
	objects[Potal_start + 18].position = XMFLOAT3(1100 + center_cube_distance, 4000 + 400, 0 + center_cube_distance);
	objects[Potal_start + 19].position = XMFLOAT3(0 + center_cube_distance, 4000 + 400, 1100 + center_cube_distance);

	for (int i = Card_start; i < Slash_end; ++i)
	{
		objects[i].tile_life = false;
	}
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
	packet.character_type = objects[obj].character_info;
	packet.hp = objects[obj].hp;
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

void ServerManager::SendAnimationPaket(unsigned short to, unsigned short obj)
{
	SCPacket_Animation packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::Animation;
	packet.animation_state = ani_state;
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
	packet.build_cube = cube_build;
	packet.position = XMFLOAT3(objects[obj].position.x, objects[obj].position.y, objects[obj].position.z);
	packet.rotate = XMFLOAT3(objects[obj].rotate.x, objects[obj].rotate.y, objects[obj].rotate.z);

	SendPacket(to, reinterpret_cast<char *>(&packet));
	//printf("%.1f, %.1f, %.1f\n", objects[obj].position.x, objects[obj].position.y, objects[obj].position.z);
}


void ServerManager::SendPotalInfoPacket(unsigned short to, unsigned short obj)
{
	SCPacket_PotalInfo packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::Potal;
	packet.build_portal = portal_build;
	packet.position = XMFLOAT3(objects[obj].position.x, objects[obj].position.y, objects[obj].position.z);
	packet.rotate = XMFLOAT3(objects[obj].rotate.x, objects[obj].rotate.y, objects[obj].rotate.z);

	SendPacket(to, reinterpret_cast<char *>(&packet));
}


void ServerManager::SendCardPaket(unsigned short to, unsigned short obj)
{
	SCPacket_ProjectTile packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::ProjectTile;
	packet.build_projecttile = projecttile_build;
	packet.position = XMFLOAT3(objects[obj].position.x, objects[obj].position.y, objects[obj].position.z);
	packet.projectTile_type = ProjectTile::Card;

	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendHitPaket(unsigned short to, unsigned short obj)
{
	SCPacket_Hit packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::OnHit;
	packet.hp = objects[obj].hp;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendSlashPaket(unsigned short to, unsigned short obj)
{
	SCPacket_ProjectTile packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::ProjectTile;
	packet.build_projecttile = projecttile_build;
	packet.position = XMFLOAT3(objects[obj].position.x, objects[obj].position.y, objects[obj].position.z);
	packet.projectTile_type = ProjectTile::Slash;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

// 로비에서 룸 생성
void ServerManager::SendRoomPacket(unsigned short to, unsigned short obj)
{
	// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	SCPacket_CreateRoom packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::CreateRoom;
	packet.player_num = player_num;
	packet.room_num = room_num;
	//packet.scene = scene;
	packet.check = check;
	SendPacket(to, reinterpret_cast<char *>(&packet));

}
// 로비에서 룸 입장
void ServerManager::SendRoomEnterPacket(unsigned short to, unsigned short obj)
{
	// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	SCPacket_EnterRoom packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::EnterRoom;
	packet.player_num = member_num;
	packet.room_num = room_num;
	packet.check = check;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendRoomExitPacket(unsigned short to, unsigned short obj)
{
	// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	SCPacket_EnterRoom packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::ExitRoom;
	packet.player_num = member_num;
	packet.room_num = room_num;
	packet.check = check;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}
void ServerManager::SendChagne_L_RPacket(unsigned short to, unsigned short obj)
{
	// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	SCPacket_EnterRoom packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::ChangeScene_L_R;
	packet.player_num = scene_member_num;
	packet.room_num = scene_room_num;
	packet.check = check;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendChagne_R_LPacket(unsigned short to, unsigned short obj)
{
	// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	SCPacket_EnterRoom packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::ChangeScene_R_L;
	packet.player_num = scene_member_num;
	packet.room_num = scene_room_num;
	packet.check = check;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendInfoScenePacket(unsigned short to, unsigned short obj)
{
	// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	SCPacket_InfoScene packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::InfoScene;
	packet.scene = scene;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendRotatePacket(unsigned short to, unsigned short obj)
{
	SCPacket_Rotate packet;// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::Rotate;
	packet.x = m_fPitch;
	packet.y = m_fYaw;
	packet.z = m_fRoll;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::ProcessPacket(unsigned short int id, char * buf)
{

	CSPacket_Move * packet = reinterpret_cast<CSPacket_Move *>(buf);
	CSPacket_Rotate *rpacket= reinterpret_cast<CSPacket_Rotate *>(buf);
	XMFLOAT3 xmf3Shift = objects[id].position;
	objects[id].m_Look = packet->m_Look;
	objects[id].m_Right = packet->m_Right;
	objects[id].m_Up = packet->m_Up;

	switch (packet->type) {
	case CS_Type::Character_Info:
	{
		CSPacket_CharacterType *myTypePacket = reinterpret_cast<CSPacket_CharacterType *>(buf);
		character_type = myTypePacket->character_type;

		break;
	}
	case CS_Type::Move:
	{
	
		if (packet->dir) {
			if (packet->dir & DIR_FORWARD)
			{
				xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Look, +fDistance);
			}
			if (packet->dir & DIR_BACKWARD)
			{
				xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Look, -fDistance);
			}
			if (packet->dir & DIR_RIGHT)
			{
				xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Right, +fDistance);
			}
			if (packet->dir & DIR_LEFT)
			{
				xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Right, -fDistance);
			}

			if (packet->dir & DIR_UP&&jump_check == false)
			{
				//xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Up, +fDistance * 2);

				if (character_type == 0)
					xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Up, +fDistance * 2);
				else if (character_type == 1)
				{
					//jump_check == true;
					xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Up, 200);

				}
			}
			if (packet->dir & DIR_DOWN)
			{
				xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Up, -fDistance);

			}

		}

		break;
	}
	case CS_Type::Rotate:
	{
		CSPacket_Rotate *packet = reinterpret_cast<CSPacket_Rotate *>(buf);

		if (packet->x != 0.0f)
		{
			m_fPitch += packet->x;
			if (m_fPitch > +89.0f) { packet->x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { packet->x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (packet->y != 0.0f)
		{
			m_fYaw += packet->y;
			if (m_fYaw > 90.0f) m_fYaw -= 90.0f;
			if (m_fYaw < 0.0f) m_fYaw += 90.0f;
		}
		if (packet->z != 0.0f)
		{
			m_fRoll += packet->z;
			if (m_fRoll > +20.0f) { packet->z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { packet->z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		//m_pCamera->Rotate(x, y, z);
		if (packet->y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(packet->y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}

		m_xmf3Look = Vector3::Normalize(m_xmf3Look);
		m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
		m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (objects[i].connected == true) {
				SendRotatePacket(i, id);
			}
		}

		break;
	}
	case CS_Type::Animation:
	{

		CSPacket_Animation * packet = reinterpret_cast<CSPacket_Animation *>(buf);
		ani_state = packet->animation_state;

		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (objects[i].connected == true) {
				SendAnimationPaket(i, id);
			}
		}
		break;
	}
	case CS_Type::Attack:
	{
		CSPacket_Attack *packet = reinterpret_cast<CSPacket_Attack*>(buf);

		if (packet->attack_type == Gambler::Idle_Attack)
		{
			objects[Card_start + card_num].tile_life = true;
			objects[Card_start + card_num].position = objects[id].position;
			card_num++;

		}

		else if (packet->attack_type == GrimReaper::Slash_Wave)
		{
			objects[Slash_start + card_num].position = objects[id].position;
			objects[Slash_start + card_num].tile_life = true;

			slash_num++;
		}

		break;
	}
	case CS_Type::Room_Create:
	{
		CSPacket_RoomCreate *packet = reinterpret_cast<CSPacket_RoomCreate*>(buf);
		
		room_num = packet->room_num + 1;
		player_num = packet->player_num;
		objects[id].change_check = packet->check;
		scene = packet->scene;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (objects[i].connected == true) {
					SendRoomPacket(i, id);
			}
		}

		break;
	}
	case CS_Type::Room_Enter:
	{
		CSPacket_RoomEnter *packet = reinterpret_cast<CSPacket_RoomEnter*>(buf);
		room_num = packet->room_num;
		member_num = packet->player_num + 1; // 방에 입장한 수 
		objects[id].change_check = packet->check;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (objects[i].connected == true) {
				SendRoomEnterPacket(i, id);
			}
		}

		break;
	}
	case CS_Type::Scene_Change_L_R:
	{
		CSPacket_SceneChange_L_R *packet = reinterpret_cast<CSPacket_SceneChange_L_R*>(buf);

		scene_room_num = packet->room_num;  // 로비에서 받은 방번호
		scene_member_num = packet->player_num; // 로비에서 받은 인원수 

		objects[id].change_check = packet->check;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (objects[i].connected == true) {
				SendChagne_L_RPacket(i, id);
			}
		}
		break;
	}
	case CS_Type::Scene_Change_R_L:
	{
		CSPacket_SceneChange_R_L *packet = reinterpret_cast<CSPacket_SceneChange_R_L*>(buf);

		scene_room_num = packet->room_num;  // 로비에서 받은 방번호
		scene_member_num = packet->player_num; // 로비에서 받은 인원수 
		//printf("%d", scene_member_num);
		objects[id].change_check = packet->check;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (objects[i].connected == true) {
				SendChagne_R_LPacket(i, id);
			}
		}
		break;
	}
	case CS_Type::Room_Exit:
	{
		CSPacket_RoomExit *packet = reinterpret_cast<CSPacket_RoomExit*>(buf);
		member_num = packet->player_num - 1; // 방에 입장한 수 
		if (member_num <= 0)
		{
			member_num = 0;

		}
		
		room_num = packet->room_num;
		
		objects[id].change_check = packet->check;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (objects[i].connected == true){
				SendRoomExitPacket(i, id);
			}
		}

		break;
	}

	case CS_Type::Sceneinfo:
	{
		CSPacket_SceneInfo *packet = reinterpret_cast<CSPacket_SceneInfo*>(buf);
		scene = packet->scene;
		unsigned short id = packet->id;
		
		if (scene == 4)
		{
			SendPutPlayerPacket(id, id); // 나 자신에게 미리 알려준다.

			for (int i = 0; i < MAX_PLAYER; ++i) {
				if (false == objects[i].connected) continue;
				if (i == id) continue; // 나 자신에게 나를 알려줄 필요는 없다.
				else {
					//	objects[i].viewlist.insert(new_id);

					SendPutPlayerPacket(i, id);
				}
			}
			cube_build = true;
			portal_build = true;
			projecttile_build = true;
			for (int i = Cube_start; i < Cube_start + 50; ++i)
			{
				SendMapInfoPacket(id, i);
			}

			for (int i = Potal_start; i < Potal_end; ++i)
			{
				SendPotalInfoPacket(id, i);
			}


			for (int i = Card_start; i < Card_end; ++i)
			{
				objects[i].position = XMFLOAT3(100000, 0, 100000);
				SendCardPaket(id, i);

			}

			for (int i = Slash_start; i < Slash_end; ++i)
			{
				objects[i].position = XMFLOAT3(-100000, 0, 100000);
				SendSlashPaket(id, i);
			}
		}

		break;
	}

	default:
		serverPrint("Unknown Packet Type Error\n");
		while (true);
	}
	
	if (Collision())
	{

		/*if (packet->dir & DIR_FORWARD&&check_b == false&& check_r ==false&& check_l ==false&& check_u ==false&& check_d == false) {
		check_f = true;
		xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Look, -fDistance);

		}
		if (packet->dir & DIR_BACKWARD&&check_f==false && check_r == false && check_l == false && check_u == false && check_d == false)
		{
		check_b = true;
		xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Look, +fDistance);

		}
		if (packet->dir & DIR_RIGHT&&check_f == false && check_b == false && check_l == false && check_u == false && check_d == false) {
		check_r = true;
		xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Right, -fDistance);

		}
		if (packet->dir & DIR_LEFT&&check_f == false && check_b == false && check_r == false && check_u == false && check_d == false) {
		check_l = true;
		xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Right, +fDistance);

		}
		if (packet->dir & DIR_UP&&check_f == false && check_b == false && check_r == false && check_l == false && check_d == false) {
		check_u = true;
		xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Up, -fDistance);

		}*/
		if (packet->dir & DIR_DOWN&&check_f == false && check_b == false && check_r == false && check_l == false && check_u == false) {
			check_d = true;
			xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Up, +fDistance);
	
		}
	}
	else
	{
		check_f = false;
		check_b = false;
		check_r = false;
		check_l = false;
		check_u = false;
		check_d = false;
		
	}



	objects[id].position = xmf3Shift;

	if (objects[id].position.x > 3000.0f)
		objects[id].position.x = 3000.0f;
	if (objects[id].position.x < -3000.0f)
		objects[id].position.x = -3000.0f;
	if (objects[id].position.z > 3000.0f)
		objects[id].position.z = 3000.0f;
	if (objects[id].position.z < -3000.0f)
		objects[id].position.z = -3000.0f;
	if (objects[id].position.y > 6000.0f)
		objects[id].position.y = 6000.0f;
	if (objects[id].position.y < 0.0f)
		objects[id].position.y = 0.0f;



	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (objects[i].connected == true) {
			SendPositionPacket(i, id);

		}
	}




	// for (int i = 0; i < MAX_USER; ++i) if (true == clients[i].connected) SendPositionPacket(i, id);
}

void ServerManager::DisConnect(unsigned short int id)
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
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


bool ServerManager::Collision()
{
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (character_type == 1)   // 도박사
		{
			objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
			objects[i].colbox.Extents = XMFLOAT3(50, 200, 50);
		}
		else if (character_type == 0) // 사신
		{
			objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
			objects[i].colbox.Extents = XMFLOAT3(60, 50, 40);
		}
	}
	/*for (int i = Card_start; i < Card_end; ++i)
	{
	objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
	objects[i].colbox.Extents = XMFLOAT3(350, 350, 350);
	}*/


	for (int i = Cube_start; i < Cube_start + 5; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(0, 0, 0);
	}
	for (int i = Cube_start + 5; i < Cube_start + 15; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 500, MAX_CUBE_SIZE - 500, MAX_CUBE_SIZE - 500);
	}
	for (int i = Cube_start + 15; i < Cube_start + 35; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 450, MAX_CUBE_SIZE - 450, MAX_CUBE_SIZE - 450);
	}
	objects[Cube_start + 31].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400);
	objects[Cube_start + 32].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400);
	objects[Cube_start + 33].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400);
	objects[Cube_start + 34].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400);

	for (int i = Cube_start + 35; i < Cube_start + 45; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400);
	}
	for (int i = Cube_start + 45; i < Cube_start + 50; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 350, MAX_CUBE_SIZE - 350, MAX_CUBE_SIZE - 350);
	}

	for (int i = Cube_start; i < Cube_start + 50; ++i)
	{

		objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
		objects[i].colbox.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		//큐브 충돌
		for (int j = 0; j < MAX_PLAYER; ++j)
		{
			if (objects[j].connected)
			{
				if (objects[i].colbox.Intersects(objects[j].colbox)) {
					//objects[j].col_check = true;

					return true;
				//	printf("충돌\n");
				}
				
			}
		}
	}
	return false;

}


void ServerManager::AddTimerEvent(unsigned int id, TimerEvent::Command cmd, double second)
{
	std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch(); // 현재 시간을 double 형태로 가져온다.
	timerQueue_Lock.lock();
	timerQueue.push(TimerEvent(id, cmd, currentTime.count() + second));
	timerQueue_Lock.unlock();
}

void ServerManager::Update(unsigned long id)
{

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		objects[i].col_check = false;

	}

	//Collision();


	if (!Collision() && objects[id].connected == true)
		objects[id].position.y -= 9.8f; // 중력 


	if (objects[id].position.y <= 0)
	{
		objects[id].position.y = 0;
	}

	/*if (jump_check == true)
	{
	XMFLOAT3 prev = objects[id].position;
	XMFLOAT3 next = objects[id].position;
	next = Vector3::Add(next, objects[id].m_Up, 200);
	objects[id].position = next;
	printf("%1.f, %1.f\n", prev.y, next.y);
	if (next.y>=prev.y+300)
	{
	jump_check = false;
	}
	}*/

	// 도박사 평타 
	for (int i = Card_start; i < Card_end; ++i)
	{
		if (objects[i].tile_life == true)
		{

			objects[i].position = Vector3::Add(objects[i].position, objects[id].m_Look, 10.0f);

			//printf("%1.f, %1.f, %1.f\n", objects[i].position.x, objects[i].position.y,objects[i].position.z);
			if (abs(objects[i].position.z - objects[id].position.z) >= 1000)
			{
				objects[i].tile_life = false;
			}
			SendPositionPacket(id, i);
		}
	}
	// 사신 검기 날리기
	for (int i = Slash_start; i < Slash_end; ++i)
	{
		if (objects[i].tile_life == true)
		{
			objects[i].position = Vector3::Add(objects[i].position, objects[id].m_Look, 10.0f);
			//printf("%1.f, %1.f, %1.f\n", objects[i].position.x, objects[i].position.y,objects[i].position.z);
			if (abs(objects[i].position.z - objects[id].position.z) >= 1000)
			{
				objects[i].tile_life = false;
			}
			SendPositionPacket(id, i);
		}
	}

	// 도박사 평타  충돌
	for (int i = Card_start; i < Card_end; ++i)
	{
		objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
		objects[i].colbox.Extents = XMFLOAT3(20, 20, 20);

		for (int j = Cube_start; j < Cube_start + 50; ++j)
		{
			objects[j].colbox.Center = XMFLOAT3(objects[j].position.x, objects[j].position.y, objects[j].position.z);
			objects[j].colbox.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			if (objects[i].tile_life&&objects[i].colbox.Intersects(objects[j].colbox))
			{
				//printf("카드충돌\n");
				objects[i].position = XMFLOAT3(0, -400000, 0);
			}
		}
		for (int k = 0; k < MAX_PLAYER; ++k)
		{
			if (objects[k].connected&&objects[id].connected&&k != id&&objects[i].tile_life&&objects[i].colbox.Intersects(objects[k].colbox))
			{

				objects[k].hp -= 2;
				SendHitPaket(k, id);

				if (objects[k].hp <= 0) {
					objects[k].position = XMFLOAT3(0, 6000, 0);
					objects[k].hp = 100;
				}
			}
		}
	}

	//검기 큐브 충돌
	for (int i = Slash_start; i < Slash_end; ++i)
	{
		objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
		objects[i].colbox.Extents = XMFLOAT3(150, 50, 50);

		for (int j = Cube_start; j < Cube_start + 50; ++j)
		{
			objects[j].colbox.Center = XMFLOAT3(objects[j].position.x, objects[j].position.y, objects[j].position.z);
			objects[j].colbox.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			if (objects[i].tile_life&&objects[i].colbox.Intersects(objects[j].colbox))
			{
				//printf("검기충돌\n");
				objects[i].position = XMFLOAT3(0, -300000, 0);
			}
		}

		for (int k = 0; k < MAX_PLAYER; ++k)
		{
			if (objects[k].connected&&objects[id].connected&&k != id&&objects[i].tile_life&&objects[i].colbox.Intersects(objects[k].colbox))
			{
				;
				objects[k].hp -= 2;
				/*if(character_type=1)
					ani_state = Gambler::OnHit;
				else if(character_type=0)
					ani_state = GrimReaper::OnHit;*/
				SendHitPaket(k, id);
				printf("검기유저충돌 %1.f!\n", objects[k].hp);
				if (objects[k].hp <= 0)
				{
					objects[k].position = XMFLOAT3(0, 6000, 0);
					objects[k].hp = 100;
				}

			}
		}
	}

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		objects[id].colbox.Center = XMFLOAT3(objects[id].position.x, objects[id].position.y, objects[id].position.z);
		objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
		if (objects[id].connected&&objects[i].connected&&objects[i].colbox.Intersects(objects[id].colbox) && id != i)
		{
			//printf("플레이어 충돌!\n");
		}
	}
	for (int i = Potal_start; i < Potal_end; ++i)
	{
		objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
		objects[i].colbox.Extents = XMFLOAT3(15, 15, 15);
	}
	for (int i = Potal_start; i < Potal_start + 5; ++i)
	{
		if (objects[id].connected&&objects[i].colbox.Intersects(objects[id].colbox))
		{
			objects[id].position = XMFLOAT3(0, 6000, 0);
		}
	}

	// 맨위포탈

	if (objects[id].connected&&objects[Potal_start + 15].colbox.Intersects(objects[id].colbox))
	{
		objects[id].position = XMFLOAT3(0 - 300, 2000 + 400, -center_cube_distance - 300);
	}
	if (objects[id].connected&&objects[Potal_start + 16].colbox.Intersects(objects[id].colbox))
	{
		objects[id].position = XMFLOAT3(-1100 - 300 + center_cube_distance, 2000 + 300, 0 - 300);
	}
	if (objects[id].connected&&objects[Potal_start + 17].colbox.Intersects(objects[id].colbox))
	{
		objects[id].position = XMFLOAT3(0 - 300 + center_cube_distance, 2000 + 300, -1100 - 300);
	}
	if (objects[id].connected&&objects[Potal_start + 18].colbox.Intersects(objects[id].colbox))
	{
		objects[id].position = XMFLOAT3(0 - 300 + center_cube_distance, 2000 + 300, -1100 - 300);
	}
	if (objects[id].connected&&objects[Potal_start + 19].colbox.Intersects(objects[id].colbox))
	{
		objects[id].position = XMFLOAT3(0 - 300 + center_cube_distance, 2000 + 300, 1100 - 300);
	}
	// 중간포탈

	for (int i = Potal_start + 10; i < Potal_start + 15; ++i)
	{
		if (objects[id].connected&&objects[i].colbox.Intersects(objects[id].colbox))
			objects[id].position = XMFLOAT3(1000, 0, 1000);
	}

	//아래 포탈
	if (objects[id].connected&&objects[Potal_start + 5].colbox.Intersects(objects[id].colbox))
	{
		objects[id].position = XMFLOAT3(100, 3000 + 400, 0);
	}
	if (objects[id].connected&&objects[Potal_start + 6].colbox.Intersects(objects[id].colbox))
	{
		objects[id].position = XMFLOAT3(-1100 + 100, 3000 + 300, 0);
	}
	if (objects[id].connected&&objects[Potal_start + 7].colbox.Intersects(objects[id].colbox))
	{
		objects[id].position = XMFLOAT3(0 + 100, 3000 + 300, -1100);
	}
	if (objects[id].connected&&objects[Potal_start + 8].colbox.Intersects(objects[id].colbox))
	{
		objects[id].position = XMFLOAT3(1100 + 100, 3000 + 300, 0);
	}
	if (objects[id].connected&&objects[Potal_start + 9].colbox.Intersects(objects[id].colbox))
	{
		objects[id].position = XMFLOAT3(0 + 100, 3000 + 300, 1100);
	}

	AddTimerEvent(id);

}

