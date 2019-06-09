#include "stdafx.h"
#include "RuntimeFrameWork.h"
#include "Scene004_BattleScene.h"
#include "Object003_SkyBox.h"
#include "Object101_GrimReaperPlayer.h"
#include "Object102_GamblerPlayer.h"
#include "Object103_ElfArcherPlayer.h"
#include "Camera000_BaseCamera.h"
#include "Material.h"
#include "Object007_DiffuseCubeObject.h"
#include "Object008_HeightmapTerrain.h"
#include "ResourceManager.h"
#include "AnimationController.h"	// 로드모델 때문
#include "Object010_SlashWaveObject.h"
#include "Object011_CardObject.h"
#include "Object006_TextureRectObject.h"



BattleScene::BattleScene()
{

}


BattleScene::~BattleScene()
{
}

void BattleScene::SendChracterType(int a)
{
	if (a == 0 || a == 1 || a == 2) {
		CSPacket_CharacterType *myMovePacket = reinterpret_cast<CSPacket_CharacterType *>(m_pFramework->GetSendBuf());
		myMovePacket->size = sizeof(CSPacket_CharacterType);
		// 클라이언트가 어느 방향으로 갈 지 키입력 정보를 저장한 비트를 서버로 보내기
		myMovePacket->character_type = a;
		myMovePacket->type = CS_Type::Character_Info;
		m_pFramework->SendPacket(reinterpret_cast<char *>(myMovePacket));
	}
}

void BattleScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 2, 45); //SuperCobra(17), Gunship(2), Player:Mi24(1), Angrybot()

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	BuildLightsAndMaterials();
	
	XMFLOAT3 xmf3Scale(24.0f, 6.0f, 24.0f);
	XMFLOAT4 xmf4Color(0.1f, 0.1f, 0.1f, 0.5f);
	//m_pTerrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, _T("Texture/HeightMap.raw"), 257, 257, xmf3Scale, xmf4Color);
	//m_pTerrain->SetPosition(-3072.0f, 0.0f, -3072.0f);

	m_pFramework->GetResource()->AllModelLoad(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	cmd = 0;
	printf("<캐릭터 선택 >\n플레이어 캐릭터 선택을 위해 커맨드를 입력하세요. ( 사신 : 0, 도박사 : 1 ) >>>  ");
	scanf_s("%d", &cmd);
	switch (cmd) {
	case 0: 
	{
		GrimReaperPlayer *pPlayer = new GrimReaperPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_pPlayer = pPlayer;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			GamblerObject[i] = new GamblerPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
			m_ppOtherPlayers[i] = GamblerObject[i];
			m_ppOtherPlayers[i]->SetPosition(XMFLOAT3(-100000.0f, -100000.0f, -100000.0f));// 위치 초기화를 하긴 해야되니까 절대 안 그려질 곳에다 짱박아두자.
		}
	}
		break;
	case 1:
	{
		GamblerPlayer *pPlayer = new GamblerPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_pPlayer = pPlayer;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			ReaperObject[i] = new GrimReaperPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
			m_ppOtherPlayers[i] = ReaperObject[i];
			m_ppOtherPlayers[i]->SetPosition(XMFLOAT3(-100000.0f, -100000.0f, -100000.0f));// 위치 초기화를 하긴 해야되니까 절대 안 그려질 곳에다 짱박아두자.
		}
	}
		break;
	default:
	{
		ElfArcherPlayer *pPlayer = new ElfArcherPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_pPlayer = pPlayer;
	}
		break;
	}
	
	m_nCubeObjects = 50;
	m_ppCubeObjects = new DiffuseCubeObject*[m_nCubeObjects];
	for (unsigned int i = 0; i < m_nCubeObjects; ++i) {
		if (i < 5) m_pFramework->cubeSize[i] = MAX_CUBE_SIZE - 400;
		else if (i < 10) m_pFramework->cubeSize[i] = MAX_CUBE_SIZE - 300;
		else if (i < 20) m_pFramework->cubeSize[i] = MAX_CUBE_SIZE - 200;
		else if (i < 30) m_pFramework->cubeSize[i] = MAX_CUBE_SIZE - 100;
		else if (i < 50) m_pFramework->cubeSize[i] = MAX_CUBE_SIZE;
		m_ppCubeObjects[i] = new DiffuseCubeObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pFramework->cubeSize[i]);
	}

	slashWave = new SlashWaveObject*[Slash_end - Slash_start];
	for (unsigned int i = 0; i < Slash_end - Slash_start; ++i) {
		slashWave[i] = new SlashWaveObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	}
	
	card = new CardObject*[Card_end - Card_start];
	for (unsigned int i = 0; i < Card_end - Card_start; ++i) {
		card[i] = new CardObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	}

	m_ppPotalObjects = new DiffuseCubeObject*[Potal_end - Potal_start];
	for (unsigned int i = 0; i < Potal_end - Potal_start; ++i) {
		m_ppPotalObjects[i] = new DiffuseCubeObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 30);
	}


	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	if (cmd == 1)	// 도박사이면
		m_nObjects = 13;
	else
		m_nObjects = 5;
	m_ppObjects = new BaseObject*[m_nObjects];
	TextureRectObject *radar = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/Radar_180x180.dds", 18.0f, 18.0f);
	m_ppObjects[0] = radar;
	TextureRectObject *emptyHPgauge = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/EmptyBar_360x60.dds", 36.0f, 6.0f);
	m_ppObjects[1] = emptyHPgauge;
	TextureRectObject *emptySPgauge = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/EmptyBar_360x60.dds", 36.0f, 6.0f);
	m_ppObjects[2] = emptySPgauge;
	TextureRectObject *hpBar = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/HPBar_360x60.dds", 36.0f, 6.0f);
	m_ppObjects[3] = hpBar;
	TextureRectObject *spBar = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/SPBar_360x60.dds", 36.0f, 6.0f);
	m_ppObjects[4] = spBar;
	if (cmd == 1) {
		TextureRectObject *remain0 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/RemainCard0.dds", 48.0f, 18.0f);
		m_ppObjects[5] = remain0;
		TextureRectObject *remain1 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/RemainCard1.dds", 48.0f, 18.0f);
		m_ppObjects[6] = remain1;
		TextureRectObject *remain2 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/RemainCard2.dds", 48.0f, 18.0f);
		m_ppObjects[7] = remain2;
		TextureRectObject *remain3 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/RemainCard3.dds", 48.0f, 18.0f);
		m_ppObjects[8] = remain3;
		TextureRectObject *remain4 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/RemainCard4.dds", 48.0f, 18.0f);
		m_ppObjects[9] = remain4;
		TextureRectObject *remain5 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/RemainCard5.dds", 48.0f, 18.0f);
		m_ppObjects[10] = remain5;
		TextureRectObject *remain6 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/RemainCard6.dds", 48.0f, 18.0f);
		m_ppObjects[11] = remain6;
		TextureRectObject *remain7 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/RemainCard7.dds", 48.0f, 18.0f);
		m_ppObjects[12] = remain7;
	}
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void BattleScene::ReleaseObjects()
{
	if (m_pGraphicsRootSignature) m_pGraphicsRootSignature->Release();

	if (m_pSkyBox) delete m_pSkyBox;

	ReleaseShaderVariables();
}

void BattleScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();

	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();

	if (m_ppObjects) {
		for (unsigned int i = 0; i < m_nObjects ; ++i)
			if (m_ppObjects[i])
				m_ppObjects[i]->ReleaseUploadBuffers();
	}
}

bool BattleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pPlayer) m_pPlayer->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:

		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:

		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
	return false;
}

bool BattleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pPlayer) m_pPlayer->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	return false;
}

bool BattleScene::ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed)
{
	if (m_pPlayer) m_pPlayer->ProcessInput(pKeysBuffer, fTimeElapsed);
	return false;
}

void BattleScene::AnimateObjects(float fTimeElapsed)
{
	SendMoveDirection();
	SendAnimationInfo();
	
	if (m_pPlayer) m_pPlayer->Animate(fTimeElapsed);

	for (int i = 0; i < MAX_PLAYER; ++i)
		if (m_ppOtherPlayers[i]->connected)
			m_ppOtherPlayers[i]->Animate(fTimeElapsed);


	auto reflection = Vector3::Subtract(XMFLOAT3(0.0f, 0.0f, 0.0f), m_pPlayer->GetCamera()->GetLookVector());
	if (m_ppObjects)
		for (unsigned int i = 0; i < m_nObjects; ++i)
			if (m_ppObjects[i]) {
				//	m_ppObjects[i]->SetLookAt(m_pPlayer->GetCamera()->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
			}

	// 레이더
	m_ppObjects[0]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 75.0f, m_pPlayer->GetCamera()->GetPosition().y + 35.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);

	// 빈 HP바
	m_ppObjects[1]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x - 50.0f, m_pPlayer->GetCamera()->GetPosition().y + 30.0f, m_pPlayer->GetCamera()->GetPosition().z + 50.0f);
	
	// 빈 SP 바
	m_ppObjects[2]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x - 50.0f, m_pPlayer->GetCamera()->GetPosition().y + 20.0f, m_pPlayer->GetCamera()->GetPosition().z + 49.0f);
	
	// HP 바
	m_ppObjects[3]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x - 50.0f, m_pPlayer->GetCamera()->GetPosition().y + 30.0f, m_pPlayer->GetCamera()->GetPosition().z + 50.0f);
	
	// SP 바
	m_ppObjects[4]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x - 50.0f, m_pPlayer->GetCamera()->GetPosition().y + 20.0f, m_pPlayer->GetCamera()->GetPosition().z + 49.0f);
	
	if (m_nObjects > 5) {
		m_ppObjects[5]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_ppObjects[6]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_ppObjects[7]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_ppObjects[8]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_ppObjects[9]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_ppObjects[10]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_ppObjects[11]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_ppObjects[12]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
	}
	/*for (int i = 0; i < m_nCubeObjects; ++i) {
		m_ppCubeObjects[i]->SetPosition(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y - 60, m_pPlayer->GetPosition().z - 50);
	}*/

	// for (unsigned int i = 0; i < Slash_end - Slash_start; ++i) if (slashWave && slashWave[i]) slashWave[i]->Animate(fTimeElapsed);
	//for (unsigned int i = 0; i < Card_end - Card_start; ++i) if (card && card[i]) card[i]->Animate(fTimeElapsed);
}

void BattleScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);

	if (m_pPlayer) m_pPlayer->Render(pd3dCommandList, pCamera);
	
	for (int i = 0; i < MAX_PLAYER; ++i)
		if (m_ppOtherPlayers[i]->connected) 
			m_ppOtherPlayers[i]->Render(pd3dCommandList, pCamera);


	for (unsigned int i = 0; i < m_nCubeObjects; ++i)
		if (m_ppCubeObjects && m_ppCubeObjects[i])
			m_ppCubeObjects[i]->Render(pd3dCommandList, pCamera);

	for (unsigned int i = 0; i < Slash_end - Slash_start; ++i) 
		if (slashWave && slashWave[i])
			slashWave[i]->Render(pd3dCommandList, pCamera);
	
	for (unsigned int i = 0; i < Card_end - Card_start; ++i) {
		if (card && card[i])
			card[i]->Render(pd3dCommandList, pCamera);
	}

	for (unsigned int i = 0; i < Potal_end - Potal_start; ++i) {
		if (m_ppPotalObjects && m_ppPotalObjects[i])
			m_ppPotalObjects[i]->Render(pd3dCommandList, pCamera);
	}

	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);


	if (m_ppObjects) for (unsigned int i = 0; i < m_nObjects; ++i) if (m_ppObjects[i]) m_ppObjects[i]->Render(pd3dCommandList, pCamera);
	
}

void BattleScene::BuildCube()
{
	if (!isBuilded) {
		for (unsigned int i = 0; i < m_nCubeObjects; ++i) {
#ifdef USE_CONSOLE_WINDOW
			// printf("UP Cube [%d] %.2f -  Pos : (%.2f, %.2f, %.2f) / Rot : (%.2f, %.2f, %.2f)\n", i, m_pFramework->cubeSize[i], m_pFramework->cubePos[i].x, m_pFramework->cubePos[i].y, m_pFramework->cubePos[i].z, m_pFramework->cubeRot[i].x, m_pFramework->cubeRot[i].y, m_pFramework->cubeRot[i].z);
#endif
			m_ppCubeObjects[i]->SetPosition(m_pFramework->cubePos[i]);
			//m_ppCubeObjects[i]->SetPosition(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y, m_pPlayer->GetPosition().z);
			m_ppCubeObjects[i]->Rotate(m_pFramework->cubeRot[i].x, m_pFramework->cubeRot[i].y, m_pFramework->cubeRot[i].z);
		}

		for (unsigned int i = 0; i < Slash_end - Slash_start; ++i)
			if (slashWave && slashWave[i])
				slashWave[i]->SetPosition(m_pFramework->slashWavePos[i]);

		for (unsigned int i = 0; i < Card_end - Card_start; ++i)
			if (card && card[i])
				card[i]->SetPosition(m_pFramework->cardPos[i]);

		for (unsigned int i = 0; i < Potal_end - Potal_start; ++i)
			if (m_ppPotalObjects&& m_ppPotalObjects[i])
				m_ppPotalObjects[i]->SetPosition(m_pFramework->potalPos[i]);
		

		isBuilded = true;
	}
}

void BattleScene::ProcessPacket(char * ptr)
{
	static bool first_time = true;
	
	switch (ptr[1])
	{
	case SC_Type::LoginOK:
	{
		//printf("login\n");
		SCPacket_LoginOK *packet = reinterpret_cast<SCPacket_LoginOK *>(ptr);
		myid = packet->id;
#ifdef USE_CONSOLE_WINDOW
		printf("LOGIN\n");
#endif
		break;
	}
	case SC_Type::PutPlayer:
	{
#ifdef USE_CONSOLE_WINDOW
		printf("Put Player\n");
#endif
		SCPacket_PutPlayer *my_packet = reinterpret_cast<SCPacket_PutPlayer *>(ptr);
		unsigned int id = my_packet->id;
		if (first_time) {
			first_time = false;
			myid = id;
		}
		if (id == myid) {
			m_pPlayer->SetVisible(true);
			m_pPlayer->SetPosition((XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z)));
			m_pPlayer->hp = my_packet->hp;

			CSPacket_CharacterType *myTypePacket = reinterpret_cast<CSPacket_CharacterType *>(m_pFramework->GetSendBuf());
			myTypePacket->size = sizeof(CSPacket_CharacterType);
			// 클라이언트가 어느 방향으로 갈 지 키입력 정보를 저장한 비트를 서버로 보내기
			myTypePacket->character_type = cmd;
			myTypePacket->type = CS_Type::Character_Info;
			m_pFramework->SendPacket(reinterpret_cast<char *>(myTypePacket));
		}
		else if (id < MAX_PLAYER) {
			if (m_ppOtherPlayers[id]) {
				m_ppOtherPlayers[id]->connected = true;
				m_ppOtherPlayers[id]->SetPosition((XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z)));
				m_ppOtherPlayers[id]->hp = my_packet->hp;
			}

		}
		
		break;
	}
	case SC_Type::Position:
	{
		SCPacket_Position *my_packet = reinterpret_cast<SCPacket_Position *>(ptr);
		unsigned short other_id = my_packet->id;
		anime = my_packet->animation_state;
		//printf("%d", anime);
	//	printf("포지션! 서버한테 받기 성공\n");
		if (other_id == myid) {
			m_pPlayer->SetVisible(true);
			//printf("Your [%d] : (%.1f, %.1f, %.1f)\n", my_packet->id, my_packet->position.x, my_packet->position.y, my_packet->position.z);
			m_pPlayer->SetPosition(my_packet->position);
		}
		else if (other_id < MAX_PLAYER) {
			//m_ppOtherPlayers[other_id]->m_pSkinnedAnimationController->SetAnimationSet(anime);
			m_ppOtherPlayers[other_id]->SetPosition(my_packet->position);
//#ifdef USE_CONSOLE_WINDOW
		}
		else if (other_id>=Card_start&&other_id<Card_end) {
			//printf("도박사 평타 : %1.f, %1.f, %1.f\n", my_packet->position.x, my_packet->position.y, my_packet->position.z);
			// 그려주시고 위치 설정
			if (card && card[other_id - Card_start]) {
				card[other_id - Card_start]->SetPosition(my_packet->position);
				//printf("도박사 평타 받은 후  : %1.f, %1.f, %1.f\n", card[other_id - Card_start]->GetPosition().x, card[other_id - Card_start]->GetPosition().y, card[other_id - Card_start]->GetPosition().z);
			}
		}
		else if (other_id >= Slash_start&&other_id < Slash_end) {
			//printf("검기 : %1.f, %1.f, %1.f\n", my_packet->position.x, my_packet->position.y, my_packet->position.z);
			// 그려주시고 위치 설정
			if (slashWave && slashWave[other_id - Slash_start]) {
				slashWave[other_id - Slash_start]->SetPosition(my_packet->position);
				//printf("검기 받은 후 : %1.f, %1.f, %1.f\n", slashWave[other_id - Slash_start]->GetPosition().x, slashWave[other_id - Slash_start]->GetPosition().y, slashWave[other_id - Slash_start]->GetPosition().z);
			}
		}
		break;
	}
	case SC_Type::RemovePlayer:
	{
#ifdef USE_CONSOLE_WINDOW
		printf("Remove\n");
#endif
//		SCPacket_RemovePlayer *my_packet = reinterpret_cast<SCPacket_RemovePlayer *>(ptr);
//		unsigned int other_id = my_packet->id;
//		if (other_id == myid) m_pPlayer->SetVisible(false);
//		if (other_id < MAX_USER) {
//			if (m_ppOtherPlayers[other_id]) {
//				//m_ppOtherPlayers[other_id]->SetVisible(false);
//				delete m_ppOtherPlayers[other_id];
//				m_ppOtherPlayers[other_id] = nullptr;
//#ifdef USE_CONSOLE_WINDOW
//				printf("Player [%d] Remove from Screen\n", my_packet->id);
//#endif
//			}
//		}
		
		break;
	}
	case SC_Type::Animation:
	{
		SCPacket_Animation *my_packet = reinterpret_cast<SCPacket_Animation*>(ptr);
		unsigned short other_id = my_packet->id;
		anime = my_packet->animation_state;

		if (other_id < MAX_PLAYER) {
			m_ppOtherPlayers[other_id]->m_pSkinnedAnimationController->SetAnimationSet(anime);
			//printf("서버한테 받음 : %d\n", anime);
		}
		//printf("어택! 서버한테 받기 성공\n");
		break;
	}
	case SC_Type::MapInfo:
	{
		//처리
		SCPacket_MapInfo *my_packet = reinterpret_cast<SCPacket_MapInfo *>(ptr);

		unsigned short id = my_packet->id - Cube_start;
		m_pFramework->cubePos[id] = XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z);
		m_pFramework->cubeRot[id] = XMFLOAT3(my_packet->rotate.x, my_packet->rotate.y, my_packet->rotate.z);
		break;
	}
	case SC_Type::Potal:
	{
		SCPacket_PotalInfo *my_packet = reinterpret_cast<SCPacket_PotalInfo *>(ptr);
		unsigned short id = my_packet->id - Potal_start;
		m_pFramework->potalPos[id] = XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z);
		break;
	}
	case SC_Type::ProjectTile:
	{		
		SCPacket_ProjectTile *my_packet = reinterpret_cast<SCPacket_ProjectTile *>(ptr);
		if (my_packet->projectTile_type == ProjectTile::Card)
		{
			//printf("card (%.1f, %.1f, %.1f)\n", my_packet->position.x, my_packet->position.y, my_packet->position.z);
			unsigned short card_id = my_packet->id - Card_start;
			m_pFramework->cardPos[card_id] = my_packet->position;
			my_packet->position = m_pPlayer[myid].GetPosition();
		}
		if (my_packet->projectTile_type == ProjectTile::Slash)
		{
			//printf("slash (%.1f, %.1f, %.1f)\n", my_packet->position.x, my_packet->position.y, my_packet->position.z);
			unsigned short slash_id = my_packet->id - Slash_start;
			m_pFramework->slashWavePos[slash_id] = my_packet->position;
			my_packet->position = m_pPlayer[myid].GetPosition();
		}
		break;
	}

	case SC_Type::OnHit:
	{
		SCPacket_Hit *my_packet = reinterpret_cast<SCPacket_Hit *>(ptr);
		unsigned short other_id = my_packet->id;
		if (other_id == myid) {
			m_pPlayer->hp = my_packet->hp;
			
			
		}
		else if (other_id < MAX_PLAYER) {
			m_ppOtherPlayers[other_id]->hp = my_packet->hp;
			
		}
		break;
	}
	default:
#ifdef USE_CONSOLE_WINDOW
		printf("Unknown PACKET type [%d]\n", ptr[1]);
#endif
		break;
	}
}
