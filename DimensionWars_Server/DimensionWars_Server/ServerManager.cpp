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
		update_check += 1;
		
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

		if (objects[new_id].connected == true)
		{
			//AddTimerEvent(new_id);
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

	for (int i = Cube_start; i < Cube_start + 50; ++i)
	{
		objects[i].position = XMFLOAT3(startPos(dre), startYPos(dre), startPos(dre));
	}
	/*objects[Cube_start +0].position = XMFLOAT3(-2000, 0, -2000);
	objects[Cube_start +5].position = XMFLOAT3(-1500, 0, -2000);
	objects[Cube_start+15].position = XMFLOAT3(-500, 0, -2000);
	objects[Cube_start+35].position = XMFLOAT3(500, 0, -2000);
	objects[Cube_start+45].position = XMFLOAT3(1500, 0, -2000);*/
	for (int i = Cube_start; i < Cube_start + 50; ++i)
	{
		for (int j = i + 1; j < Cube_start + 50; ++j)
		{
			if (Distance(objects[i].position, objects[j].position) < 400)
			{
				objects[i].position = XMFLOAT3(startPos(dre), startYPos(dre), startPos(dre));
				if (Distance(objects[i].position, objects[j].position) < 500)
				{
					objects[i].position = XMFLOAT3(startPos(dre), startYPos(dre), startPos(dre));
					if (Distance(objects[i].position, objects[j].position) < 600)
					{
						objects[i].position = XMFLOAT3(startPos(dre), startYPos(dre), startPos(dre));

						if (Distance(objects[i].position, objects[j].position) < 700)
						{
							objects[i].position = XMFLOAT3(startPos(dre), startYPos(dre), startPos(dre));

						}
					}
				}
			}
		}
	}

	
	
	// 맨아래
	objects[Potal_start].position = XMFLOAT3(0, 50, 0);
	objects[Potal_start + 1].position = XMFLOAT3(2500, 50, 0);
	objects[Potal_start + 2].position = XMFLOAT3(-2500, 50, 0);
	objects[Potal_start + 3].position = XMFLOAT3(0, 50, 2500);
	objects[Potal_start + 4].position = XMFLOAT3(0, 50, -2500);

	/*for (int i = 5; i < 30; ++i)
	{
		objects[Potal_start + i].position = XMFLOAT3(objects[2 * i + 1].position.x, objects[2 * i + 1].position.y, objects[2 * i + 1].position.z);
	}*/
	// 300큐브 포탈 3개
	objects[Potal_start + 5].position = XMFLOAT3(objects[11].position.x, objects[11].position.y+200, objects[11].position.z);
	objects[Potal_start + 6].position = XMFLOAT3(objects[13].position.x, objects[13].position.y+200, objects[13].position.z);
	objects[Potal_start + 7].position = XMFLOAT3(objects[15].position.x, objects[15].position.y+200, objects[15].position.z);
	
	//400큐브 포탈5개
	for (int i = 8; i < 13; ++i)
	{
		objects[Potal_start + i].position = XMFLOAT3(objects[2 * i + 1].position.x, objects[2 * i + 1].position.y+250, objects[2 * i + 1].position.z);
	}
	// 500큐브 포탈 10개
	for (int i = 13; i < 23; ++i)
	{
		objects[Potal_start + i].position = XMFLOAT3(objects[2 * i + 1].position.x, objects[2 * i + 1].position.y + 300, objects[2 * i + 1].position.z);
	}
	// 600큐브 포탈 5개
	for (int i = 23; i < 28; ++i)
	{
		objects[Potal_start + i].position = XMFLOAT3(objects[2 * i + 1].position.x, objects[2 * i + 1].position.y + 350, objects[2 * i + 1].position.z);
	}
	// 700큐브 포탈 2개
	for (int i = 28; i <30; ++i)
	{
		objects[Potal_start + i].position = XMFLOAT3(objects[2 * i + 1].position.x, objects[2 * i + 1].position.y + 400, objects[2 * i + 1].position.z);
	}


	objects[Reaper_scy-1].position = XMFLOAT3(0,0,-3000);
	for (int i = Card_start; i < Slash_end; ++i)
	{
		objects[i].tile_life = false;
	}
	for (int i = Arrow_start; i < Arrow_end; ++i)
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
	packet.sp = objects[obj].sp;
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

void ServerManager::SendArrowPaket(unsigned short to, unsigned short obj)
{
	SCPacket_ProjectTile packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::ProjectTile;
	packet.build_projecttile = projecttile_build;
	packet.position = XMFLOAT3(objects[obj].position.x, objects[obj].position.y, objects[obj].position.z);
	packet.projectTile_type = ProjectTile::Arrow;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendNattackPaket(unsigned short to, unsigned short obj)
{
	SCPacket_Attack packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::Attack;
	packet.position = XMFLOAT3(objects[obj].position.x, objects[obj].position.y, objects[obj].position.z);
	
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

void ServerManager::SendRotatePacket(unsigned short to, unsigned short obj, float x, float y, float z)
{
	SCPacket_Rotate packet;// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::Rotate;
	packet.x = x;
	packet.y = y;
	packet.z = z;
	packet.m_Look = objects[obj].m_Look;
	packet.m_Right = objects[obj].m_Right;
	packet.m_Up = objects[obj].m_Up;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}
void ServerManager::SendOtherCharacterPacket(unsigned short to, unsigned short obj)
{
	SCPacket_OtherCharacter packet;// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_Type::Chracter_type;
	packet.character_type = other_ctype;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendGameTimePaket(unsigned short to)
{
	SCPacket_GameTime packet;
	packet.id = to;
	packet.size = sizeof(packet);
	packet.type = SC_Type::GameTime;
	packet.time = timecnt;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::ProcessPacket(unsigned short int id, char * buf)
{

	CSPacket_Move * packet = reinterpret_cast<CSPacket_Move *>(buf);
	CSPacket_Rotate *rpacket= reinterpret_cast<CSPacket_Rotate *>(buf);
	XMFLOAT3 xmf3Shift = objects[id].position;
	objects[id].m_Right = rpacket->m_Right;
	objects[id].m_Up = rpacket->m_Up;
	objects[id].m_Look = rpacket->m_Look;
	switch (packet->type) {
	case CS_Type::Character_Info:
	{
		CSPacket_CharacterType *myTypePacket = reinterpret_cast<CSPacket_CharacterType *>(buf);
		other_ctype = myTypePacket->character_type;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (false == objects[i].connected) continue;
			if (i == id) continue; // 나 자신에게 나를 알려줄 필요는 없다.
			else {
				
				SendOtherCharacterPacket(i, id);
			}
		}
		break;
	}
	case CS_Type::Move:
	{
		
		objects[id].m_Look = packet->m_Look;
		objects[id].m_Right = packet->m_Right;
		objects[id].m_Up = packet->m_Up;
		if (packet->dir) {
			if (packet->dir & DIR_FORWARD)
			{
				xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Look, +fDistance);
			}
			if (packet->dir & DIR_BACKWARD)
			{
				xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Look, -fDistance);
			}
			if (packet->dir & DIR_RIGHT)
			{
				xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Right, +fDistance);
			}
			if (packet->dir & DIR_LEFT)
			{
				xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Right, -fDistance);
			}

			if (packet->dir & DIR_UP&&jump_check == false)
			{
				//xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Up, +fDistance * 2);

				if (character_type == 0)
					xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Up, +fDistance * 2);
				else 
				{
					//jump_check == true;
					xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Up, 200);

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
		
		objects[id].m_Right = packet->m_Right;
		objects[id].m_Up = packet->m_Up;
		objects[id].m_Look = packet->m_Look;
		
		if (packet->x != 0.0f)
		{
			m_fPitch += packet->x;
			//if (m_fPitch > +89.0f) { packet->x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			//if (m_fPitch < -89.0f) { packet->x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (packet->y != 0.0f) 
		{
			m_fYaw += packet->y;
			//if (m_fYaw > 90.0f) m_fYaw -= 90.0f;
			//if (m_fYaw < 0.0f) m_fYaw += 90.0f;
		}
		if (packet->z != 0.0f)
		{
			m_fRoll += packet->z;
			//if (m_fRoll > +20.0f) { packet->z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			//if (m_fRoll < -20.0f) { packet->z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		//m_pCamera->Rotate(x, y, z);
		if (packet->y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&objects[id].m_Up), XMConvertToRadians(packet->x));
			objects[id].m_Look = Vector3::TransformNormal(objects[id].m_Look, xmmtxRotate);
			objects[id].m_Right = Vector3::TransformNormal(objects[id].m_Right, xmmtxRotate);
		}

		objects[id].m_Look = Vector3::Normalize(objects[id].m_Look);
		objects[id].m_Right = Vector3::CrossProduct(objects[id].m_Up, objects[id].m_Look, true);
		objects[id].m_Up = Vector3::CrossProduct(objects[id].m_Look, objects[id].m_Right, true);

		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (objects[i].connected == true) {
				SendRotatePacket(i, id, m_fPitch, m_fYaw, m_fRoll);
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

		if (objects[id].character_info == 2)
		{
			if (packet->attack_type == ElfArcher::First_Shot)
			{
				objects[Arrow_start + arrow_num].tile_life = true;
				objects[Arrow_start + arrow_num].position = objects[id].position;
				++arrow_num;
			}
		}

		if (objects[id].character_info == 1)
		{
			if (packet->attack_type == Gambler::Idle_Attack)
			{
				objects[Card_start + card_num].tile_life = true;
				objects[Card_start + card_num].position = objects[id].position;
				card_num++;

			}
		}
		if (objects[id].character_info == 0)
		{
			if (packet->attack_type == GrimReaper::Slash_Wave)
			{
				objects[Slash_start + slash_num].position = objects[id].position;
				objects[Slash_start + slash_num].tile_life = true;

				slash_num++;
				objects[id].sp -= 10;
				//printf("%f", objects[id].sp);
			}
			if (packet->attack_type == GrimReaper::First_Attack)
			{
				objects[Reaper_scy - 1].position = objects[id].position;
				hitcheck = true;
			}
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
			if (false == objects[i].connected) continue;
			if (i == id) continue; // 나 자신에게 나를 알려줄 필요는 없다.
			else {
				SendRoomEnterPacket(i, id);
			}
		}
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (false == objects[i].connected) continue;
			if (i == id) continue;
			else {
				SendRoomEnterPacket(id, i);
			}
		}
		
		break;
	}
	//case CS_Type::Scene_Change_L_R:
	//{
	//	CSPacket_SceneChange_L_R *packet = reinterpret_cast<CSPacket_SceneChange_L_R*>(buf);

	//	scene_room_num = packet->room_num;  // 로비에서 받은 방번호
	//	scene_member_num = packet->player_num; // 로비에서 받은 인원수 

	//	objects[id].change_check = packet->check;
	//	for (int i = 0; i < MAX_PLAYER; ++i) {
	//		if (objects[i].connected == true) {
	//			SendChagne_L_RPacket(i, id);
	//		}
	//	}
	//	break;
	//}
	//case CS_Type::Scene_Change_R_L:
	//{
	//	CSPacket_SceneChange_R_L *packet = reinterpret_cast<CSPacket_SceneChange_R_L*>(buf);

	//	scene_room_num = packet->room_num;  // 로비에서 받은 방번호
	//	scene_member_num = packet->player_num; // 로비에서 받은 인원수 
	//	//printf("%d", scene_member_num);
	//	objects[id].change_check = packet->check;
	//	for (int i = 0; i < MAX_PLAYER; ++i) {
	//		if (objects[i].connected == true) {
	//			SendChagne_R_LPacket(i, id);
	//		}
	//	}
	//	break;
	//}
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
		objects[id].character_info = packet->c_type;
		character_type = packet->c_type;
		//unsigned short id = packet->id;
		
		if (scene == 3)
		{
			//SendOtherCharacterPacket(id, id); // 나 자신에게 미리 알려준다.

			
			for (int i = 0; i < MAX_PLAYER; ++i) {
				if (false == objects[i].connected) continue;
				if (i == id) continue; // 나 자신에게 나를 알려줄 필요는 없다.
				else {
					//	objects[i].viewlist.insert(new_id);

					SendPutPlayerPacket(i, id);
				}
			}
			for (int i = 0; i < MAX_PLAYER; ++i) {
				if (false == objects[i].connected) continue;
				if (i == id) continue;
				else {
					//objects[new_id].viewlist.insert(i);

					SendPutPlayerPacket(id, i);
				}
			}
		}

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
			for (int i = 0; i < MAX_PLAYER; ++i) {
				if (false == objects[i].connected) continue;
				if (i == id) continue;
				else {
					//objects[new_id].viewlist.insert(i);

					SendPutPlayerPacket(id, i);
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
			for (int i = Arrow_start; i < Arrow_end; ++i)
			{
				objects[i].position = XMFLOAT3(-100000, 0, 100000);
				SendArrowPaket(id, i);
			}
			SendNattackPaket(id, Reaper_scy-1);// 사신 낫 
		}
		break;
	}

	default:
		serverPrint("Unknown Packet Type Error\n");
		while (true);
	}
	
	if (objects[id].col_check)
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
		/*if (packet->dir & DIR_DOWN) {
			
			xmf3Shift = Vector3::Add(xmf3Shift, packet->m_Up, +fDistance);
	
		}*/
		/*if (packet->dir & DIR_FORWARD) {
			
			xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Look, -fDistance);

		}
		if (packet->dir & DIR_BACKWARD)
		{
			
			xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Look, +fDistance);

		}
		if (packet->dir & DIR_RIGHT) {
			
			xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Right, -fDistance);

		}
		if (packet->dir & DIR_LEFT) {
			
			xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Right, +fDistance);

		}*/
		if (packet->dir & DIR_UP) {
			
			xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Up, -fDistance);

		}
		if (packet->dir & DIR_DOWN) {
			
			xmf3Shift = Vector3::Add(xmf3Shift, objects[id].m_Up, +fDistance);

		}
	}
	else
	{// 낙하애니메이션
		if (objects[id].position.y > 0)
		{
			if (objects[id].character_info == 2)
			{
				ani_state = ElfArcher::Fall;
			}
			else if (objects[id].character_info == 1)
			{
				ani_state = Gambler::Fall;
			}
			for (int i = 0; i < MAX_PLAYER; ++i) {
				if (objects[i].connected == true) {
					//SendAnimationPaket(i, id);
				}
			}
		}
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


void ServerManager::Collision()
{

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (objects[i].character_info == 1)   // 도박사
		{
			objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y+80, objects[i].position.z);
			objects[i].colbox.Extents = XMFLOAT3(50, 100, 50);
		}
		else if (objects[i].character_info == 0) // 사신
		{
			objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y+40, objects[i].position.z);
			objects[i].colbox.Extents = XMFLOAT3(60, 55, 60);
		}
		else if (objects[i].character_info == 2) // 엘프
		{
			objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y + 70, objects[i].position.z);
			objects[i].colbox.Extents = XMFLOAT3(60, 75, 60);
		}
	}
	objects[Reaper_scy - 1].colbox.Center = XMFLOAT3(objects[Reaper_scy - 1].position.x, objects[Reaper_scy - 1].position.y+80, objects[Reaper_scy - 1].position.z+10);
	objects[Reaper_scy - 1].colbox.Extents = XMFLOAT3(80, 10, 10);
	objects[Reaper_scy - 1].colbox.Orientation = XMFLOAT4(0, 0, 1, 1);
	/*for (int i = Card_start; i < Card_end; ++i)
	{
	objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
	objects[i].colbox.Extents = XMFLOAT3(350, 350, 350);
	}*/


	for (int i = Cube_start; i < Cube_start + 5; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(150, 150, 150);
	}
	for (int i = Cube_start + 5; i < Cube_start + 15; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(200,200,200);
	}
	for (int i = Cube_start + 15; i < Cube_start + 35; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(250,250,250);
	}
	/*objects[Cube_start + 31].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 200, MAX_CUBE_SIZE - 200, MAX_CUBE_SIZE - 200);
	objects[Cube_start + 32].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400);
	objects[Cube_start + 33].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400);
	objects[Cube_start + 34].colbox.Extents = XMFLOAT3(MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400, MAX_CUBE_SIZE - 400);
*/
	for (int i = Cube_start + 35; i < Cube_start + 45; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(300,300,300);
	}
	for (int i = Cube_start + 45; i < Cube_start + 50; ++i) {
		objects[i].colbox.Extents = XMFLOAT3(350,350,350);
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
					objects[j].col_check = true;
					//printf("충돌!\n");

					//	printf("충돌\n");
				}
				else
				{
					
				}

			}
		}
	}
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
	//if (scene == 4&&update_check==1)
	//{
	//	
	//	Game_Timer += 0.01f;
	//	if (Game_Timer >= 1.0f)
	//	{
	//		timecnt += 1.0f;
	//		Game_Timer = 0;
	//		//SendGameTimePaket(id);
	//	}
	//}
	
	objects[id].col_check = false;
	Collision();
	if (!objects[id].col_check && objects[id].connected == true)
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


	// 플레이어 충돌
	
	if (scene == 4)
	{
		
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (objects[id].connected&&objects[i].connected&&objects[i].colbox.Intersects(objects[id].colbox) && id != i)
			{
				//printf("플레이어 충돌!\n");
			}
			if (hitcheck&&objects[i].connected&&objects[i].character_info != 0 && objects[i].colbox.Intersects(objects[Reaper_scy - 1].colbox))
			{
				
				objects[i].hp -= 1.0f;
				objects[Reaper_scy - 1].position = XMFLOAT3(50000, 0, 20);
				printf("%f", objects[i].hp);
				SendHitPaket(i,id);
			
			}
		}
		
	}
	
	if (scene == 4)
	{
		// 도박사 평타 
		for (int k = 0; k < MAX_PLAYER; ++k)
		{
			for (int i = Card_start; i < Card_end; ++i)
			{
				if (objects[i].tile_life == true&&objects[k].connected==true)
				{
					card_time += 0.01f;
					objects[i].position = Vector3::Add(objects[i].position, objects[k].m_Look, 10.0f);
					if (card_time > 1.0f)
					{
						objects[i].tile_life = false;
						card_time = 0.0f;
					}
					SendPositionPacket(id, i);
				}
			}
		}
		// 사신 검기 날리기
		//printf("%f, %f, %f\n", objects[id].m_Look.x, objects[id].m_Look.y, objects[id].m_Look.z);


		for (int i = Slash_start; i < Slash_end; ++i)
		{
			if (objects[i].tile_life == true)
			{
				slash_time += 0.01f;
				objects[i].position = Vector3::Add(objects[i].position, objects[id].m_Look, 10.0f);
				//printf("%1.f, %1.f, %1.f\n", objects[i].position.x, objects[i].position.y,objects[i].position.z);
				if (slash_time > 1.0f)
				{
					objects[i].tile_life = false;
					slash_time = 0.0f;
				}
				SendPositionPacket(id, i);
			}

		}
		// 엘프 평타
		for (int i = Arrow_start; i < Arrow_end; ++i)
		{
			if (objects[i].tile_life == true)
			{
				arrow_time += 0.01f;
				objects[i].position = Vector3::Add(objects[i].position, objects[id].m_Look, 10.0f);
				//printf("%1.f, %1.f, %1.f\n", objects[i].position.x, objects[i].position.y,objects[i].position.z);
				if (arrow_time > 1.0f)
				{
					objects[i].tile_life = false;
					arrow_time = 0.0f;
				}
				SendPositionPacket(id, i);
			}

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
				
				objects[k].hp -= 2;
				/*if(character_type=1)
					ani_state = Gambler::OnHit;
				else if(character_type=0)
					ani_state = GrimReaper::OnHit;*/
				SendHitPaket(k, id);
				//printf("검기유저충돌 %1.f!\n", objects[k].hp);
				if (objects[k].hp <= 0)
				{
					objects[k].position = XMFLOAT3(0, 6000, 0);
					objects[k].hp = 100;
				}

			}
		}
	}

	
	for (int i = Potal_start; i < Potal_end; ++i)
	{
		objects[i].colbox.Center = XMFLOAT3(objects[i].position.x, objects[i].position.y, objects[i].position.z);
		objects[i].colbox.Extents = XMFLOAT3(30, 30, 30);
	}
	
	// 바닥에있는 포탈 맨위로 올라감
	for (int i = Potal_start; i < Potal_start + 5; ++i)
	{
		if (objects[id].connected&&objects[i].colbox.Intersects(objects[id].colbox))
		{
			objects[id].position = XMFLOAT3(0, 6000, 0);
		}
	}
	// 300짜리  짝수번호로 한칸씩 이동
	for (int i = 5; i < 8; ++i)
	{
		if (objects[id].connected&&objects[Potal_start+i].colbox.Intersects(objects[id].colbox))
		{
			objects[id].position = XMFLOAT3(objects[2 * i + 2].position.x, objects[2 * i + 2].position.y+300, objects[2 * i + 2].position.z);
		}
	}
	for (int i = 8; i < 13; ++i)
	{
		if (objects[id].connected&&objects[Potal_start + i].colbox.Intersects(objects[id].colbox))
		{
			objects[id].position = XMFLOAT3(objects[2 * i + 2].position.x, objects[2 * i + 2].position.y + 350, objects[2 * i + 2].position.z);
		}
	}
	for (int i = 13; i < 23; ++i)
	{
		if (objects[id].connected&&objects[Potal_start + i].colbox.Intersects(objects[id].colbox))
		{
			objects[id].position = XMFLOAT3(objects[2 * i + 2].position.x, objects[2 * i + 2].position.y + 400, objects[2 * i + 2].position.z);
		}
	}
	for (int i = 23; i < 28; ++i)
	{
		if (objects[id].connected&&objects[Potal_start + i].colbox.Intersects(objects[id].colbox))
		{
			objects[id].position = XMFLOAT3(objects[2 * i + 2].position.x, objects[2 * i + 2].position.y + 450, objects[2 * i + 2].position.z);
		}
	}
	for (int i = 28; i < 30; ++i)
	{
		if (objects[id].connected&&objects[Potal_start + i].colbox.Intersects(objects[id].colbox))
		{
			objects[id].position = XMFLOAT3(objects[2 * i + 2].position.x, objects[2 * i + 2].position.y + 500, objects[2 * i + 2].position.z);
		}
	}

	
	AddTimerEvent(id);

}

