#include "stdafx.h"
#include "Scene003_RoomScene.h"
#include "Material.h"
#include "Object006_TextureRectObject.h"
#include "Camera000_BaseCamera.h"
#include "RuntimeFrameWork.h"
#include "Texture.h"
#include "Shader005_TextureRectangleShader.h"
#include "ResourceManager.h"
#include "Object104_DummyPlayer.h"
#include "Scene004_BattleScene.h"
#include "Object013_ScreenTextureObject.h"
#include "Shader009_UIShader.h"
RoomScene::RoomScene()
{
}


RoomScene::~RoomScene()
{
}

void RoomScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_nObjects3 = 19; 
	m_roomObjects = new BaseObject*[m_nObjects3];
	//
	nCurrScene = 2;

	Object104_DummyPlayer *pPlayer = new Object104_DummyPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	m_pPlayer = pPlayer;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		DummyObject[i] = new Object104_DummyPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_ppOtherPlayers[i] = DummyObject[i];
		m_ppOtherPlayers[i]->SetPosition(XMFLOAT3(-100000.0f, -100000.0f, -100000.0f));// 위치 초기화를 하긴 해야되니까 절대 안 그려질 곳에다 짱박아두자.
	}
	Texture *roomImage[room_texture];

	roomImage[0] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[0]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/Room_bg2.dds", 0);

	roomImage[1] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[1]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/List_bg.dds", 0);

	roomImage[2] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[2]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/Position_bg.dds", 0);

	roomImage[3] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[3]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/left_not_act.dds", 0);

	roomImage[4] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[4]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/right_not_act.dds", 0);

	roomImage[5] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[5]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/left_act.dds", 0);

	roomImage[6] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[6]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/right_act.dds", 0);

	roomImage[7] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[7]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/Room_exit.dds", 0);

	roomImage[8] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[8]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/Start.dds", 0);

	roomImage[9] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[9]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/GrimReaper_select.dds", 0);

	roomImage[10] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[10]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/Gambler_select.dds", 0);

	roomImage[11] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[11]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/Elf_select.dds", 0);

	roomImage[12] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[12]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/Void_select.dds", 0);

	roomImage[13] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[13]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/Start_not.dds", 0);



	
	

	UIShader *pTextureShader = new UIShader();
	pTextureShader->CreateShader(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature);
	pTextureShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	
	for (int i = 0; i < room_texture; ++i)
		CreateShaderResourceViews(pd3dDevice, pd3dCommandList, roomImage[i], 15, true);


	//Material *roomMaterial[room_texture];

	
	for (int i = 0; i < room_texture; ++i)
	{
		roomMaterial[i] = new Material(1);
		roomMaterial[i]->SetTexture(roomImage[i]);
		roomMaterial[i]->SetShader(pTextureShader);
	}
	

	// 룸배경
	ScreenTextureObject *roomImageObject = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature,-1.0f,1.0f,1.0f,-1.0f);
	m_roomObjects[0] = roomImageObject;
	m_roomObjects[0]->SetMaterial(0, roomMaterial[0]);
	// 내 캐릭터 선택배경
	ScreenTextureObject *roomImageObject1 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.9f,0.5f,-0.6f,-0.1f);
	m_roomObjects[1] = roomImageObject1;
	m_roomObjects[1]->SetMaterial(0, roomMaterial[1]);
	// 상대 캐릭터 배경
	ScreenTextureObject *roomImageObject2 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.5f, 0.8f, 0.9f, -0.5f);
	m_roomObjects[2] = roomImageObject2;
	m_roomObjects[2]->SetMaterial(0, roomMaterial[2]);
	// 왼쪽 비활성
	ScreenTextureObject *roomImageObject3 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.9f, -0.2f, -0.8f, -0.3f);
	m_roomObjects[3] = roomImageObject3;
	m_roomObjects[3]->SetMaterial(0, roomMaterial[3]);
	// 오른쪽 비활성
	ScreenTextureObject *roomImageObject4 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.7f, -0.2f, -0.6f, -0.3f);
	m_roomObjects[4] = roomImageObject4;
	m_roomObjects[4]->SetMaterial(0, roomMaterial[4]);
	// 왼쪽 활성 
	ScreenTextureObject *roomImageObject5 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.9f, -0.2f, -0.8f, -0.3f);
	m_roomObjects[5] = roomImageObject5;
	m_roomObjects[5]->SetMaterial(0, roomMaterial[5]);
	// 오른쪽 활성
	ScreenTextureObject *roomImageObject6 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.7f, -0.2f, -0.6f, -0.3f);
	m_roomObjects[6] = roomImageObject6;
	m_roomObjects[6]->SetMaterial(0, roomMaterial[6]);
	// 방 나가기
	ScreenTextureObject *roomImageObject7 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.9f, -0.75f, -0.7f, -0.95f);
	m_roomObjects[7] = roomImageObject7;
	m_roomObjects[7]->SetMaterial(0, roomMaterial[7]);
	// 게임시작
	ScreenTextureObject *roomImageObject8 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.7f, -0.75f, 0.9f, -0.95f);
	m_roomObjects[8] = roomImageObject8;
	m_roomObjects[8]->SetMaterial(0, roomMaterial[8]);
	//빈 플레이어
	ScreenTextureObject *roomImageObject9 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.45f, 0.75f, -0.05f, 0.1f);
	m_roomObjects[9] = roomImageObject9;
	m_roomObjects[9]->SetMaterial(0, roomMaterial[12]);
	
	ScreenTextureObject *roomImageObject10 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.0f, 0.75f, 0.4f, 0.1f);
	m_roomObjects[10] = roomImageObject10;
	m_roomObjects[10]->SetMaterial(0, roomMaterial[12]);

	ScreenTextureObject *roomImageObject11 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.45f, 0.75f, 0.85f, 0.1f);
	m_roomObjects[11] = roomImageObject11;
	m_roomObjects[11]->SetMaterial(0, roomMaterial[12]);

	ScreenTextureObject *roomImageObject12 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.45f, 0.1f, -0.05f, -0.55f);
	m_roomObjects[12] = roomImageObject12;
	m_roomObjects[12]->SetMaterial(0, roomMaterial[12]);

	ScreenTextureObject *roomImageObject13 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.0f, 0.1f, 0.4f, -0.55f);
	m_roomObjects[13] = roomImageObject13;
	m_roomObjects[13]->SetMaterial(0, roomMaterial[12]);

	ScreenTextureObject *roomImageObject14 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.45f, 0.1f, 0.85f, -0.55f);
	m_roomObjects[14] = roomImageObject14;
	m_roomObjects[14]->SetMaterial(0, roomMaterial[12]);
	// 내캐릭터 변경
	ScreenTextureObject *roomImageObject15 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.89f, 0.4f, -0.61f, -0.1f);
	m_roomObjects[15] = roomImageObject15;
	m_roomObjects[15]->SetMaterial(0, roomMaterial[9]);

	ScreenTextureObject *roomImageObject16 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.89f, 0.4f, -0.61f, -0.1f);
	m_roomObjects[16] = roomImageObject16;
	m_roomObjects[16]->SetMaterial(0, roomMaterial[10]);

	ScreenTextureObject *roomImageObject17 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.89f, 0.4f, -0.61f, -0.1f);
	m_roomObjects[17] = roomImageObject17;
	m_roomObjects[17]->SetMaterial(0, roomMaterial[11]);
	// 비활성 게임시작 버튼
	ScreenTextureObject *roomImageObject18 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.7f, -0.75f, 0.9f, -0.95f);
	m_roomObjects[18] = roomImageObject18;
	m_roomObjects[18]->SetMaterial(0, roomMaterial[13]);
	
}

void RoomScene::ReleaseObjects()
{
}

void RoomScene::ReleaseUploadBuffers()
{
	if (m_roomObjects) {
		for (int i = 0; i < m_nObjects3; ++i) {
			m_roomObjects[i]->ReleaseUploadBuffers();
		}
	}
}

bool RoomScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		//나가기
		if (pt.x > 55 && pt.x < 150 && pt.y>674 && pt.y < 744)
		{
			room_exit = true;
			SendRoomExit();
			m_pFramework->ChangeScene(BaseScene::SceneTag::Lobby);
		}
		// 입장
		if (pt.x > 874 && pt.x < 969 && pt.y>674 && pt.y < 744)
		{
			//SendReady();
			if (ready == 0)
			{
				ready = 1;
			}
			else if (ready == 1)
			{
				ready = 0;
			}
			SendSceneInfo(4, c_type);
			//m_pFramework->m_pCommandList->Reset(m_pFramework->m_pCommandAllocator, NULL);
			//m_pFramework->resourceMgr = new ResourceManager();
			////m_pPlayer = m_pFramework->arrScene[BaseScene::SceneTag::Game]->m_pPlayer;
			//m_pFramework->arrScene[BaseScene::SceneTag::Game]->BuildObjects(m_pFramework->m_pDevice, m_pFramework->m_pCommandList);
			//m_pFramework->m_pPlayer = m_pFramework->arrScene[BaseScene::SceneTag::Title]->m_pPlayer;
			//m_pCamera = m_pPlayer->GetCamera();
			//m_pFramework->m_pCommandList->Close();
			//m_pFramework->BuildObjects2();

			m_pFramework->ChangeScene(BaseScene::SceneTag::Game);
			
		}
		if (ready == 0)
		{
			//좌측 화살표
			if (pt.x > 55 && pt.x < 100 && pt.y>465 && pt.y < 490)
			{
				--c_type;
				if (c_type > 2)
				{
					c_type = 2;
				}
				SendOtherCharacter();
			}

			//우측 화살표
			if (pt.x > 155 && pt.x < 200 && pt.y>465 && pt.y < 490)
			{
				++c_type;
				if (c_type > 2)
				{
					c_type = 0;
				}
				SendOtherCharacter();
			}
		}
		break;
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

bool RoomScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	
	return false;
}

bool RoomScene::ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed)
{
	return false;
}

void RoomScene::AnimateObjects(float fTimeElapsed)
{
	//좌측
	if (pt.x > 69 && pt.x < 111 && pt.y>460 && pt.y < 485)
		left_act = true;
	else
		left_act = false;
	//우측 화살표
	if (pt.x > 154 && pt.x < 197 && pt.y>460 && pt.y < 485)
		right_act = true;
	else
		right_act = false;
	// 나가기
	if (pt.x > 55 && pt.x < 180 && pt.y>666 && pt.y < 726)
		m_roomObjects[7]->SetPosition(-240, -180, -25);
	else
		m_roomObjects[7]->SetPosition(-240, -190, -25);
	// 시작
	if (pt.x > 841 && pt.x < 966 && pt.y>666 && pt.y < 726)
		m_roomObjects[8]->SetPosition(240, -180, -25);
	else
		m_roomObjects[8]->SetPosition(240, -190, -25);
	
	
	// 내가 방생성해서 들어갔을때
	if (m_pFramework->enter_type == 0)
	{
		
		if (c_type == Character_type::Reaper)
		{
			m_roomObjects[9]->SetMaterial(0, roomMaterial[9]);
		}
		if (c_type == Character_type::Gamber)
		{
			m_roomObjects[9]->SetMaterial(0, roomMaterial[10]);
		}
		if (c_type == Character_type::Elf)
		{
			m_roomObjects[9]->SetMaterial(0, roomMaterial[11]);
		}
		
		if (m_pFramework->nBase_member[0] < 1)
		{
			m_roomObjects[9]->SetMaterial(0, roomMaterial[12]);
		}
		if (m_pFramework->nBase_member[0] < 2)
		{
			m_roomObjects[10]->SetMaterial(0, roomMaterial[12]);
		}

		if (m_pFramework->nBase_member[0] >= 2)
		{
			if (m_ppOtherPlayers[other_id]->character_type == Character_type::Reaper)
			{
				m_roomObjects[10]->SetMaterial(0, roomMaterial[9]);
			}
			if (m_ppOtherPlayers[other_id]->character_type == Character_type::Gamber)
			{
				m_roomObjects[10]->SetMaterial(0, roomMaterial[10]);
			}
			if (m_ppOtherPlayers[other_id]->character_type == Character_type::Elf)
			{
				m_roomObjects[10]->SetMaterial(0, roomMaterial[11]);
			}
			
		}
		if (m_pFramework->nBase_member[0] >= 3)
		{
			if (m_ppOtherPlayers[other_player[1]]->character_type == Character_type::Reaper)
			{
				m_roomObjects[11]->SetMaterial(0, roomMaterial[9]);
			}
			if (m_ppOtherPlayers[other_player[1]]->character_type == Character_type::Gamber)
			{
				m_roomObjects[11]->SetMaterial(0, roomMaterial[10]);
			}
			if (m_ppOtherPlayers[other_player[1]]->character_type == Character_type::Elf)
			{
				m_roomObjects[11]->SetMaterial(0, roomMaterial[11]);
			}
			
		}
		if (m_pFramework->nBase_member[0] == 4)
		{
			
		}
		if (m_pFramework->nBase_member[0] == 5)
		{
			
		}
		if (m_pFramework->nBase_member[0] == 6)
		{
			
		}
		
		
	}
	// 생성된 방에 들어 갔을 때
	if (m_pFramework->enter_type == 1)
	{


		if (m_ppOtherPlayers[other_id]->character_type == Character_type::Reaper)
		{
			m_roomObjects[9]->SetMaterial(0, roomMaterial[9]);
		}
		if (m_ppOtherPlayers[other_id]->character_type == Character_type::Gamber)
		{
			m_roomObjects[9]->SetMaterial(0, roomMaterial[10]);
		}
		if (m_ppOtherPlayers[other_id]->character_type == Character_type::Elf)
		{
			m_roomObjects[9]->SetMaterial(0, roomMaterial[11]);
		}

		if (m_pFramework->nBase_member[0] < 1)
		{
			m_roomObjects[10]->SetMaterial(0, roomMaterial[12]);
		}
		if (m_pFramework->nBase_member[0] < 2)
		{
			m_roomObjects[9]->SetMaterial(0, roomMaterial[12]);
		}

		if (m_pFramework->nBase_member[0] == 2 && m_pFramework->select_space[0] == 0)
		{
			if (c_type == Character_type::Reaper)
			{
				m_roomObjects[10]->SetMaterial(0, roomMaterial[9]);
			}
			if (c_type == Character_type::Gamber)
			{
				m_roomObjects[10]->SetMaterial(0, roomMaterial[10]);
			}
			if (c_type == Character_type::Elf)
			{
				m_roomObjects[10]->SetMaterial(0, roomMaterial[11]);
			}
			
		}
		if (m_pFramework->nBase_member[0] == 3 && m_pFramework->select_space[1] == 0)
		{
			if (c_type == Character_type::Reaper)
			{
				m_roomObjects[11]->SetMaterial(0, roomMaterial[9]);
			}
			if (c_type == Character_type::Gamber)
			{
				m_roomObjects[11]->SetMaterial(0, roomMaterial[10]);
			}
			if (c_type == Character_type::Elf)
			{
				m_roomObjects[11]->SetMaterial(0, roomMaterial[11]);
			}
			
		}
		if (m_pFramework->nBase_member[0] == 4 && m_pFramework->select_space[2] == 0)
		{
			//m_roomObjects[18 + m_pFramework->myid]->SetPosition(-80, -20, -10);
			m_pFramework->select_space[0] = 1;
			m_pFramework->select_space[1] = 1;
			m_pFramework->select_space[3] = 1;
			m_pFramework->select_space[4] = 1;
			
		}
		if (m_pFramework->nBase_member[0] == 5 && m_pFramework->select_space[3] == 0)
		{
			//m_roomObjects[18 + m_pFramework->myid]->SetPosition(60, -20, -10);
			m_pFramework->select_space[0] = 1;
			m_pFramework->select_space[1] = 1;
			m_pFramework->select_space[2] = 1;
			m_pFramework->select_space[4] = 1;
			
		}
		if (m_pFramework->nBase_member[0] == 6 && m_pFramework->select_space[4] == 0)
		{
			//m_roomObjects[18 + m_pFramework->myid]->SetPosition(200, -20, -10);
			m_pFramework->select_space[0] = 1;
			m_pFramework->select_space[1] = 1;
			m_pFramework->select_space[2] = 1;
			m_pFramework->select_space[3] = 1;
			
		}
	}
	
}

void RoomScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	m_pFramework->SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);



	if (m_roomObjects)
	{
		m_roomObjects[7]->Render(pd3dCommandList, pCamera);
		if (ready == 0)
			m_roomObjects[18]->Render(pd3dCommandList, pCamera);
		else
			m_roomObjects[8]->Render(pd3dCommandList, pCamera);

		// 내캐릭 셀렉
		
		if (c_type == Character_type::Reaper)
		{
			m_roomObjects[15]->Render(pd3dCommandList, pCamera);
		}
		if (c_type == Character_type::Gamber)
		{
			m_roomObjects[16]->Render(pd3dCommandList, pCamera);
		}
		if (c_type == Character_type::Elf)
		{
			m_roomObjects[17]->Render(pd3dCommandList, pCamera);
		}

		/*if (c_type == Character_type::Reaper)
			m_roomObjects[18]->Render(pd3dCommandList, pCamera);
		if (c_type == Character_type::Gamber)
			m_roomObjects[19]->Render(pd3dCommandList, pCamera);
		if (c_type == Character_type::Elf)
			m_roomObjects[20]->Render(pd3dCommandList, pCamera);*/

		



		if (left_act == true)
			m_roomObjects[5]->Render(pd3dCommandList, pCamera);
		else
			m_roomObjects[3]->Render(pd3dCommandList, pCamera);

		if (right_act == true)
			m_roomObjects[6]->Render(pd3dCommandList, pCamera);
		else
			m_roomObjects[4]->Render(pd3dCommandList, pCamera);


		for (int i = 0; i < 6; ++i)
		{
			m_roomObjects[i + 9]->Render(pd3dCommandList, pCamera);
		}
		m_roomObjects[1]->Render(pd3dCommandList, pCamera);
		m_roomObjects[2]->Render(pd3dCommandList, pCamera);
		m_roomObjects[0]->Render(pd3dCommandList, pCamera);
	}
}

void RoomScene::ProcessPacket(char * ptr)
{
	static bool first_time = true;

	switch (ptr[1])
	{
	case SC_Type::Chracter_type:
	{
		SCPacket_OtherCharacter *my_packet = reinterpret_cast<SCPacket_OtherCharacter*>(ptr);
		//other_id = my_packet->id;
		if (first_time) {
			first_time = false;
			//m_pFramework->myid = other_id;
		}
		if (other_id == m_pFramework->myid) {
			
		}
		else if (other_id < MAX_PLAYER&& other_id != m_pFramework->myid) {
			if (m_ppOtherPlayers[other_id]) {
				m_ppOtherPlayers[other_id]->n_member = other_id;
				m_ppOtherPlayers[other_id]->connected = true;
				m_ppOtherPlayers[other_id]->character_type = my_packet->character_type;
				
				//other_player[other_id] = other_id;
			}

		}
		
		break;
	}
	case SC_Type::PutPlayer:
	{
		SCPacket_PutPlayer *my_packet = reinterpret_cast<SCPacket_PutPlayer *>(ptr);
		other_id = my_packet->id;
		//printf("%d\n", other_id);
		if (first_time) {
			first_time = false;
			//m_pFramework->myid = other_id;
		}
		if (other_id == m_pFramework->myid) {

		}
		else if (other_id < MAX_PLAYER&& other_id != m_pFramework->myid) {
			if (m_ppOtherPlayers[other_id]) {
				
				m_ppOtherPlayers[other_id]->n_member = other_id;
				m_ppOtherPlayers[other_id]->connected = true;
				m_ppOtherPlayers[other_id]->character_type = my_packet->character_type;
				m_roomObjects[9 + other_id]->c_type = my_packet->character_type;
				
				
			}

		}
		break;
	}
	case SC_Type::CreateRoom:
	{

		SCPacket_CreateRoom *packet = reinterpret_cast<SCPacket_CreateRoom*>(ptr);
		
		
		break;
	}
	case SC_Type::EnterRoom:
	{

		SCPacket_EnterRoom *packet = reinterpret_cast<SCPacket_EnterRoom*>(ptr);
		
		m_pFramework->room_num = packet->room_num;
		m_pFramework->nBase_room[m_pFramework->room_num - 1] = packet->room_num; // n번방
		m_pFramework->nBase_member[m_pFramework->room_num - 1] = packet->player_num; // n번방의 인원수
		
		break;
	}
	case SC_Type::ExitRoom:
	{
		SCPacket_ExitRoom *packet = reinterpret_cast<SCPacket_ExitRoom*>(ptr);
		m_pFramework->room_num = packet->room_num;
		if (packet->player_num == 0)
		{
			m_pFramework->nBase_room[m_pFramework->room_num] = 0;  // 룸번호
			m_pFramework->nBase_member[m_pFramework->room_num ] = packet->player_num; // 룸에 있는 인원
		}
		else 
		{
			m_pFramework->nBase_room[m_pFramework->room_num - 1] = packet->room_num;
			m_pFramework->nBase_member[m_pFramework->room_num - 1] = packet->player_num; // 룸에 있는 인원
		}
		//m_pFramework->nBase_room[m_pFramework->room_num - 1] = packet->room_num;  // 룸번호
		if (room_exit == true)
		{
			//SendRomm_LobbyChange();
			//m_pFramework->ChangeScene(BaseScene::SceneTag::Lobby);
			
			room_exit = false;
		}
		break;
	}
	
	case SC_Type::Position:
	{
		SCPacket_Position *my_packet = reinterpret_cast<SCPacket_Position *>(ptr);
		
		break;
	}
	case SC_Type::RemovePlayer:
	{
		break;
	}
	case SC_Type::Animation:
	{
		SCPacket_Animation *my_packet = reinterpret_cast<SCPacket_Animation*>(ptr);
		
		break;
	}
	case SC_Type::MapInfo:
	{
		//처리
		SCPacket_MapInfo *my_packet = reinterpret_cast<SCPacket_MapInfo *>(ptr);

		break;
	}
	case SC_Type::Potal:
	{
		SCPacket_PotalInfo *my_packet = reinterpret_cast<SCPacket_PotalInfo *>(ptr);
		break;
	}
	case SC_Type::ProjectTile:
	{
		SCPacket_ProjectTile *my_packet = reinterpret_cast<SCPacket_ProjectTile *>(ptr);
		
		break;
	}

	case SC_Type::OnHit:
	{
		SCPacket_Hit *my_packet = reinterpret_cast<SCPacket_Hit *>(ptr);
		
		break;
	}

	case SC_Type::ReadyGame:
	{
		
		SCPacket_ReadyGame *my_packet = reinterpret_cast<SCPacket_ReadyGame *>(ptr);
		if (my_packet->ready_state == 1)
		{
			SendSceneInfo(4, c_type);
			m_pFramework->ChangeScene(BaseScene::SceneTag::Game);
			
		}

		break;
	}

	default:
#ifdef USE_CONSOLE_WINDOW
		printf("룸Unknown PACKET type [%d]\n", ptr[1]);
#endif
		break;
	}
}

void RoomScene::SendRomm_LobbyChange()
{
	CSPacket_SceneChange_R_L *myPacket = reinterpret_cast<CSPacket_SceneChange_R_L*>(m_pFramework->GetSendBuf());
	myPacket->size = sizeof(CSPacket_SceneChange_R_L);
	myPacket->type = CS_Type::Scene_Change_R_L;
	myPacket->player_num = m_pFramework->nBase_member[m_pFramework->room_num -1];// 현재 인원수
	myPacket->room_num = m_pFramework->nBase_room[m_pFramework->room_num -1];  // // 방 번호
	myPacket->scene = BaseScene::SceneTag::Room;
	myPacket->check = room_exit;
	m_pFramework->SendPacket(reinterpret_cast<char *>(myPacket));
	//printf("룸에서 보낼때%d, %d, %d\n", room_num, m_pFramework->nBase_room[room_num - 1], m_pFramework->nBase_room[room_num - 1]);

}

void RoomScene::SendRoomExit()
{
	CSPacket_RoomExit *myPacket = reinterpret_cast<CSPacket_RoomExit*>(m_pFramework->GetSendBuf());
	myPacket->size = sizeof(CSPacket_RoomExit);
	myPacket->type = CS_Type::Room_Exit;
	myPacket->player_num = m_pFramework->nBase_member[m_pFramework->room_num - 1];// 현재 인원수
	myPacket->room_num = m_pFramework->nBase_room[m_pFramework->room_num - 1];  // // 방 번호
	myPacket->scene = BaseScene::SceneTag::Room;
	myPacket->check = room_exit;
	m_pFramework->SendPacket(reinterpret_cast<char *>(myPacket));
	
	//printf("%d, %d, %d\n", m_pFramework->room_num, m_pFramework->nBase_room[m_pFramework->room_num - 1], m_pFramework->nBase_room[m_pFramework->room_num - 1]);

}


void RoomScene::SendOtherCharacter()
{
	CSPacket_CharacterType *myPacket = reinterpret_cast<CSPacket_CharacterType*>(m_pFramework->GetSendBuf());
	myPacket->size = sizeof(CSPacket_CharacterType);
	myPacket->type = CS_Type::Character_Info;
	myPacket->character_type = c_type;
	m_pFramework->SendPacket(reinterpret_cast<char *>(myPacket));

	//printf("%d, %d, %d\n", m_pFramework->room_num, m_pFramework->nBase_room[m_pFramework->room_num - 1], m_pFramework->nBase_room[m_pFramework->room_num - 1]);

}

void RoomScene::SendReady()
{
	CSPacket_GameReady *myPacket = reinterpret_cast<CSPacket_GameReady*>(m_pFramework->GetSendBuf());
	myPacket->size = sizeof(CSPacket_GameReady);
	myPacket->type = CS_Type::GameReady;
	myPacket->ready_state = ready;
	m_pFramework->SendPacket(reinterpret_cast<char *>(myPacket));

	//printf("%d, %d, %d\n", m_pFramework->room_num, m_pFramework->nBase_room[m_pFramework->room_num - 1], m_pFramework->nBase_room[m_pFramework->room_num - 1]);

}