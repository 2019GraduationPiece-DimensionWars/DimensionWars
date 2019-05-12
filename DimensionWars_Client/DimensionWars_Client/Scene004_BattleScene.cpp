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

BattleScene::BattleScene()
{

}


BattleScene::~BattleScene()
{
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

	int cmd = 0;
	printf("임시 캐릭터 입력 : 사신 - 0, 도박사 - 1\n");
	scanf_s("%d", &cmd);

	switch (cmd) {
	case 0: 
	{
		GrimReaperPlayer *pPlayer = new GrimReaperPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_pPlayer = pPlayer;
	}
		break;
	case 1:
	{
		GamblerPlayer *pPlayer = new GamblerPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_pPlayer = pPlayer;
	}
		break;
	default:
	{
		ElfArcherPlayer *pPlayer = new ElfArcherPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_pPlayer = pPlayer;
	}
		break;
	}

	for (int i = 0; i < MAX_PLAYER; ++i) {
		ReaperObject[i] = new GrimReaperPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		GamblerObject[i] = new GamblerPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		ElfObject[i] = new ElfArcherPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_ppOtherPlayers[i] = GamblerObject[i];
		m_ppOtherPlayers[i]->SetPosition(XMFLOAT3(-100000.0f, -100000.0f, -100000.0f));// 위치 초기화를 하긴 해야되니까 절대 안 그려질 곳에다 짱박아두자.
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

	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

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
	
	if (m_pPlayer) m_pPlayer->Animate(fTimeElapsed);

	for (int i = 0; i < MAX_PLAYER; ++i)
		if (m_ppOtherPlayers[i]->connected)
			m_ppOtherPlayers[i]->Animate(fTimeElapsed);
	
	/*for (int i = 0; i < m_nCubeObjects; ++i) {
		m_ppCubeObjects[i]->SetPosition(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y - 60, m_pPlayer->GetPosition().z - 50);
	}*/
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

	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
}

void BattleScene::BuildCube()
{
	if (!isBuilded) {
		for (unsigned int i = 0; i < m_nCubeObjects; ++i) {
#ifdef USE_CONSOLE_WINDOW
			// printf("UP Cube [%d] %.2f -  Pos : (%.2f, %.2f, %.2f) / Rot : (%.2f, %.2f, %.2f)\n", i, m_pFramework->cubeSize[i], m_pFramework->cubePos[i].x, m_pFramework->cubePos[i].y, m_pFramework->cubePos[i].z, m_pFramework->cubeRot[i].x, m_pFramework->cubeRot[i].y, m_pFramework->cubeRot[i].z);
#endif
			m_ppCubeObjects[i]->SetPosition(m_pFramework->cubePos[i].x, m_pFramework->cubePos[i].y, m_pFramework->cubePos[i].z);
			//m_ppCubeObjects[i]->SetPosition(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y, m_pPlayer->GetPosition().z);
			m_ppCubeObjects[i]->Rotate(m_pFramework->cubeRot[i].x, m_pFramework->cubeRot[i].y, m_pFramework->cubeRot[i].z);
		}
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
			m_pPlayer->SetPosition((XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z)));
			//m_pPlayer->SetVisible(true);
#ifdef USE_CONSOLE_WINDOW
			printf("Your [%d] : (%.1f, %.1f, %.1f)\n", my_packet->id, my_packet->position.x, my_packet->position.y, my_packet->position.z);
#endif
		}
		else if (id < MAX_PLAYER) {
			if (m_ppOtherPlayers[id]) {
				m_ppOtherPlayers[id]->connected = true;
				m_ppOtherPlayers[id]->SetPosition((XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z)));
			}
#ifdef USE_CONSOLE_WINDOW
			printf("Put Player [%d]  (%.1f, %.1f %.1f)\n", my_packet->id, my_packet->position.x, my_packet->position.y, my_packet->position.z);
#endif
		}

		break;
	}
	case SC_Type::Position:
	{
		SCPacket_Position *my_packet = reinterpret_cast<SCPacket_Position *>(ptr);
		unsigned short other_id = my_packet->id;
		if (other_id == myid) {
			//printf("Your [%d] : (%.1f, %.1f, %.1f)\n", my_packet->id, my_packet->position.x, my_packet->position.y, my_packet->position.z);
			m_pPlayer->SetPosition((XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z)));
		}
		else if (other_id < MAX_PLAYER) {
			m_ppOtherPlayers[other_id]->SetPosition((XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z)));
#ifdef USE_CONSOLE_WINDOW
			printf("other [%d] : (%.1f, %.1f, %.1f)\n", my_packet->id, my_packet->position.x, my_packet->position.y, my_packet->position.z);
#endif
		}
		break;
	}
	case SC_Type::RemovePlayer:
	{
#ifdef USE_CONSOLE_WINDOW
		printf("Remove\n");
#endif
		SCPacket_RemovePlayer *my_packet = reinterpret_cast<SCPacket_RemovePlayer *>(ptr);
		unsigned int other_id = my_packet->id;
		//if (other_id == myid) m_pPlayer->SetVisible(false);
		/*else if (other_id < MAX_USER) {
		if (m_ppOtherPlayers[other_id]) {
		m_ppOtherPlayers[other_id]->SetVisible(false);
		delete m_ppOtherPlayers[other_id];
		m_ppOtherPlayers[other_id] = nullptr;
		#ifdef USE_CONSOLE_WINDOW
		printf("Player [%d] Remove from Screen\n", my_packet->id);
		#endif
		}
		}
		else {
		if (m_ppNonePlayerObject[other_id - NPC_ID_START]) {
		m_ppNonePlayerObject[other_id - NPC_ID_START]->SetVisible(false);
		delete m_ppNonePlayerObject[other_id - NPC_ID_START];
		m_ppNonePlayerObject[other_id - NPC_ID_START] = nullptr;
		}
		}*/
		break;
	}
	case SC_Type::MapInfo:
	{
		//처리
		SCPacket_MapInfo *my_packet = reinterpret_cast<SCPacket_MapInfo *>(ptr);

		unsigned short id = my_packet->id - Cube_start;
		m_pFramework->cubePos[id] = XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z);
		m_pFramework->cubeRot[id] = XMFLOAT3(my_packet->rotate.x, my_packet->rotate.y, my_packet->rotate.z);

#ifdef USE_CONSOLE_WINDOW
		// printf("MAPINFO Cube [%d] %.2f -  Pos : (%.2f, %.2f, %.2f) / Rot : (%.2f, %.2f, %.2f)\n", id, m_pFramework->cubeSize[id], m_pFramework->cubePos[id].x, m_pFramework->cubePos[id].y, m_pFramework->cubePos[id].z, m_pFramework->cubeRot[id].x, m_pFramework->cubeRot[id].y, m_pFramework->cubeRot[id].z);
#endif
		break;
	}
	default:
#ifdef USE_CONSOLE_WINDOW
		printf("Unknown PACKET type [%d]\n", ptr[1]);
#endif
	}
}
