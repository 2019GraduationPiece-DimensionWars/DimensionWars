#include "stdafx.h"
#include "Scene002_LobbyScene.h"
#include "Material.h"
#include "Object006_TextureRectObject.h"
#include "Camera000_BaseCamera.h"
#include "RuntimeFrameWork.h"
#include "Object100_BasePlayer.h"
#include "Texture.h"
#include "Shader005_TextureRectangleShader.h"
#include "Object104_DummyPlayer.h"
#include "Shader007_TerrainShader.h"
#include "Object014_ScreenTextureObject.h"
#include "Shader009_UIShader.h"


LobbyScene::LobbyScene()
{
}


LobbyScene::~LobbyScene()
{
}

void LobbyScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	//m_pFramework->m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 2, 45);
	nCurrScene = 1;

	m_nObjects2 = 55; //텍스쳐 로드는 45개
	m_lobbyObjects = new BaseObject*[m_nObjects2];
	//

	DummyPlayer *pPlayer = new DummyPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	m_pPlayer = pPlayer;
	
	Texture *lobbyImage[n_texture];
	
	lobbyImage[0] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[0]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/lobby_bg.dds", 0);

	lobbyImage[1] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[1]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/list_bg.dds", 0);

	lobbyImage[2] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[2]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/left_not_act.dds", 0);

	lobbyImage[3] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[3]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/right_not_act.dds", 0);

	lobbyImage[4] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[4]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/left_act.dds", 0);

	lobbyImage[5] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[5]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/right_act.dds", 0);

	lobbyImage[6] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[6]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/room_ct.dds", 0);

	lobbyImage[7] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[7]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/room_list2.dds", 0);

	lobbyImage[8] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[8]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/n_member1.dds", 0);

	lobbyImage[9] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[9]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/n_member2.dds", 0);

	lobbyImage[10] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[10]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/n_member3.dds", 0);

	lobbyImage[11] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[11]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/n_member4.dds", 0);

	lobbyImage[12] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[12]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/n_member5.dds", 0);

	lobbyImage[13] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	lobbyImage[13]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Lobby/n_member6.dds", 0);
	

	UIShader *pTextureShader = new UIShader();
	pTextureShader->CreateShader(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature);
	pTextureShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	for (int i = 0; i < n_texture; ++i)
	{
		CreateShaderResourceViews(pd3dDevice, pd3dCommandList, lobbyImage[i], 15, true);
	}
	
	
	Material *lobbyMaterial[n_texture];
	
	for (int i = 0; i < n_texture; ++i)
	{
		lobbyMaterial[i] = new Material(1);
		lobbyMaterial[i]->SetTexture(lobbyImage[i]);
		lobbyMaterial[i]->SetShader(pTextureShader);

	}
	

	// 현재 화살표 안씀
	
	
	// 로비배경
	ScreenTextureObject *lobbyImageObject = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -1.0f, 1.0f, 1.0f,-1.0f);
	m_lobbyObjects[0] = lobbyImageObject;
	m_lobbyObjects[0]->SetMaterial(0, lobbyMaterial[0]);
	//m_lobbyObjects[0]->SetPosition(0, 0, 0);
	// 방 배경
	ScreenTextureObject *lobbyImageObject1 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature,-0.9f,0.9f,0.9f,-0.7f);
	m_lobbyObjects[1] = lobbyImageObject1;
	m_lobbyObjects[1]->SetMaterial(0, lobbyMaterial[1]);
	//m_lobbyObjects[1]->SetPosition(0, 25, 1);
	// 왼쪽 비활성
	ScreenTextureObject *lobbyImageObject2 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature,-1.0f,1.0f,1.0f,-1.0f);
	m_lobbyObjects[2] = lobbyImageObject2;
	m_lobbyObjects[2]->SetMaterial(0, lobbyMaterial[2]);
	//m_lobbyObjects[2]->SetPosition(-60, -170, -22);
	// 오른쪽 비활성
	ScreenTextureObject *lobbyImageObject3 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature,  40, 20);
	m_lobbyObjects[3] = lobbyImageObject3;
	m_lobbyObjects[3]->SetMaterial(0, lobbyMaterial[3]);
	m_lobbyObjects[3]->SetPosition(60, -170, -22);
	// 왼쪽 활성
	ScreenTextureObject *lobbyImageObject4 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature,  40, 20);
	m_lobbyObjects[4] = lobbyImageObject4;
	m_lobbyObjects[4]->SetMaterial(0, lobbyMaterial[4]);
	m_lobbyObjects[4]->SetPosition(-60, -170, -22);
	// 오른쪽 활성
	ScreenTextureObject *lobbyImageObject5 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 40, 20);
	m_lobbyObjects[5] = lobbyImageObject5;
	m_lobbyObjects[5]->SetMaterial(0, lobbyMaterial[5]);
	m_lobbyObjects[5]->SetPosition(60, -170, -22);
	
	// 방 생성
	ScreenTextureObject *lobbyImageObject6 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.7f, -0.75f, 0.9f, -0.95f);
	m_lobbyObjects[6] = lobbyImageObject6;
	m_lobbyObjects[6]->SetMaterial(0, lobbyMaterial[6]);
	//m_lobbyObjects[6]->SetPosition(230, -190, -25);

	// 방 목록
	for (int i = 0; i < 12; ++i)
	{
		ScreenTextureObject *lobbyImageObject7 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, -0.8f, 0.8f - 0.25f*i, 0.8f, 0.6f - 0.25f*i);
		m_lobbyObjects[i+7] = lobbyImageObject7;
		m_lobbyObjects[i+7]->SetMaterial(0, lobbyMaterial[7]);
		//m_lobbyObjects[i]->SetPosition(0, 150, -25);
	}

	// 방에 입장한 플레이어 수 
	for (int i = 19; i < 25; ++i)
	{
		ScreenTextureObject *lobbyImageObject8 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.65f, 0.8f, 0.75f, 0.6f);
		m_lobbyObjects[i] = lobbyImageObject8;
		m_lobbyObjects[i]->SetMaterial(0, lobbyMaterial[i - 11]);
	}

	for (int i = 25; i < 31; ++i)
	{
		ScreenTextureObject *lobbyImageObject8 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.65f, 0.55f, 0.75f, 0.35f);
		m_lobbyObjects[i] = lobbyImageObject8;
		m_lobbyObjects[i]->SetMaterial(0, lobbyMaterial[i - 17]);
	}

	for (int i = 31; i < 37; ++i)
	{
		ScreenTextureObject *lobbyImageObject8 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.65f, 0.3f, 0.75f, 0.1f);
		m_lobbyObjects[i] = lobbyImageObject8;
		m_lobbyObjects[i]->SetMaterial(0, lobbyMaterial[i - 23]);
	}

	for (int i = 37; i < 43; ++i)
	{
		ScreenTextureObject *lobbyImageObject8 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.65f, 0.05f, 0.75f, -0.15f);
		m_lobbyObjects[i] = lobbyImageObject8;
		m_lobbyObjects[i]->SetMaterial(0, lobbyMaterial[i - 29]);
	}

	for (int i = 43; i < 49; ++i)
	{
		ScreenTextureObject *lobbyImageObject8 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.65f, -0.2f, 0.75f, -0.4f);
		m_lobbyObjects[i] = lobbyImageObject8;
		m_lobbyObjects[i]->SetMaterial(0, lobbyMaterial[i - 35]);
	}

	for (int i = 49; i < 55; ++i)
	{
		ScreenTextureObject *lobbyImageObject8 = new ScreenTextureObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 0.65f, -0.45f, 0.75f, -0.65f);
		m_lobbyObjects[i] = lobbyImageObject8;
		m_lobbyObjects[i]->SetMaterial(0, lobbyMaterial[i - 41]);
	}

	
	
	
	//for (int i = 0; i < m_nObjects2; ++i)
	//{
	//	m_lobbyObjects[i]->Rotate(-8, 0, 0);
	//	//m_lobbyObjects[i]->SetPosition(m_lobbyObjects[i]->GetPosition().x, m_lobbyObjects[i]->GetPosition().y + 200.0f, m_lobbyObjects[i]->GetPosition().z);
	//}
	
	
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void LobbyScene::ReleaseObjects()
{
}

void LobbyScene::ReleaseUploadBuffers()
{
	if (m_lobbyObjects) {
		for (unsigned int i = 0; i < m_nObjects2; ++i) {
			m_lobbyObjects[i]->ReleaseUploadBuffers();
		}
	}
}

bool LobbyScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		//ConsolePrint("%d, %d\n", pt.x, pt.y);
		//좌측 화살표
		//if (pt.x > 380 && pt.x < 445 && pt.y>650 && pt.y < 680)
		//{
		//	list_num = 1;
		//}
		////우측 화살표
		//if (pt.x > 580 && pt.x < 640 && pt.y>650 && pt.y < 680)
		//{
		//	///if(room_num>5)
		//		list_num = 2;
		//}
		// 방 생성
		if (pt.x > 872 && pt.x < 972 && pt.y>673 && pt.y < 743)
		{
			//room_num += 1;
			if (m_pFramework->room_num > 6)
			{
				//list_num = 2;
			}
			
			if (m_pFramework->room_num < 6) {

				room_enter = true;
				SendRoomCreate();
				SendSceneInfo(3,0);
				m_pFramework->enter_type = 0;
				//m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
			//	SendLobby_RoomChange(1);

			}
			
		}
		// 방 리스트 클릭
		if (list_num == 1) {
			if (pt.x > 106 && pt.x < 906 && pt.y>80 && pt.y < 140)
			{
				if (m_lobbyObjects[7]->room_name == my_room_num) {
					if (m_pFramework->nBase_member[0] < 6&& m_pFramework->nBase_member[0]!=0)
					{
						
						room_enter = true;
						SendEnterRoom();
						SendSceneInfo(3,0);
						m_pFramework->enter_type = 1;
						//SendLobby_RoomChange(my_room_num);
					}
					
				}
			}
			if (pt.x > 106 && pt.x < 906 && pt.y>177 && pt.y < 237)
			{
				if (m_lobbyObjects[8]->room_name == my_room_num) {
					if (m_pFramework->nBase_member[1] < 6 && m_pFramework->nBase_member[1] != 0)
					{
						room_enter = true;
						m_pFramework->enter_type = 1;
						SendSceneInfo(3, 0);
						SendEnterRoom();
						//SendLobby_RoomChange(my_room_num);
					}
				}
			}
			if (pt.x > 106 && pt.x < 906 && pt.y>273 && pt.y < 333)
			{
				if (m_lobbyObjects[9]->room_name == my_room_num) {
					if (m_pFramework->nBase_member[2]< 6 && m_pFramework->nBase_member[2] != 0)
					{
						room_enter = true;
						m_pFramework->enter_type = 1;
						SendSceneInfo(3, 0);
						SendEnterRoom();
						//SendLobby_RoomChange(my_room_num);
					}
				}
			}
			if (pt.x > 106 && pt.x < 906 && pt.y>368 && pt.y < 428)
			{
				if (m_lobbyObjects[10]->room_name == my_room_num) {
					if (m_pFramework->nBase_member[3] < 6 && m_pFramework->nBase_member[3] != 0)
					{
						room_enter = true;
						m_pFramework->enter_type = 1;
						SendSceneInfo(3, 0);
						SendEnterRoom();
						//SendLobby_RoomChange(my_room_num);
					}
				}
			}
			if (pt.x > 106 && pt.x < 906 && pt.y>463 && pt.y < 523)
			{
				if (m_lobbyObjects[11]->room_name == my_room_num) {
					if (m_pFramework->nBase_member[4] < 6 && m_pFramework->nBase_member[4] != 0)
					{
						room_enter = true;
						m_pFramework->enter_type = 1;
						SendSceneInfo(3,  0);
						SendEnterRoom();
						//SendLobby_RoomChange(my_room_num);
					}
				}
			}
			if (pt.x > 106 && pt.x < 906 && pt.y>561 && pt.y < 621)
			{
				if (m_lobbyObjects[12]->room_name == my_room_num) {
					if (m_pFramework->nBase_member[5] < 6 && m_pFramework->nBase_member[5] != 0)
					{
						room_enter = true;
						m_pFramework->enter_type = 1;
						SendSceneInfo(3,  0);
						SendEnterRoom();
						//SendLobby_RoomChange(my_room_num);
					}
				}
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

bool LobbyScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool LobbyScene::ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed)
{
	return false;
}

void LobbyScene::AnimateObjects(float fTimeElapsed)
{
	//좌측 화살표
	//if (pt.x > 380 && pt.x < 445 && pt.y>650 && pt.y < 680)
	//	left_active = true;
	//else
	//	left_active = false;
	////우측 화살표
	//if (pt.x > 580 && pt.x < 640 && pt.y>650 && pt.y < 680)
	//	right_active = true;
	//else
	//	right_active = false;
	// 방 생성
	if (pt.x > 872 && pt.x < 972 && pt.y>673 && pt.y < 743)
		m_lobbyObjects[6]->SetPosition(0.5f,0.5f,0);
	
	else
		m_lobbyObjects[6]->SetPosition(0.1f,0.1f,0);

	// 방 목록
	for (int i = 0; i < 2; ++i)
	{
		if (list_num == i+1) {
			if (pt.x > 106 && pt.x < 906 && pt.y>80 && pt.y < 140)
			{
				my_room_num = 1;
				m_lobbyObjects[7+6*i]->SetPosition(0, 160, -25);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[19+k]->SetPosition(195, 158, -34);
				}
			}
			else
			{
				m_lobbyObjects[7 + 6 * i]->SetPosition(0, 150, -25);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[19 + k]->SetPosition(195, 148, -34);
				}
			}
			//2R
			if (pt.x > 106 && pt.x < 906 && pt.y>177 && pt.y < 237)
			{
				my_room_num = 2;
				m_lobbyObjects[8 + 6 * i]->SetPosition(0, 110, -33);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[25 + k]->SetPosition(195, 108, -42);
				}
			}
			else
			{
				m_lobbyObjects[8 + 6 * i]->SetPosition(0, 100, -33);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[25 + k]->SetPosition(195, 98, -42);
				}
			}
			//3R
			if (pt.x > 106 && pt.x < 906 && pt.y>273 && pt.y < 333)
			{
				my_room_num = 3;
				m_lobbyObjects[9 + 6 * i]->SetPosition(0, 60, -41);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[31 + k]->SetPosition(195, 58, -50);
				}
			}
			else
			{
				m_lobbyObjects[9 + 6 * i]->SetPosition(0, 50, -41);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[31 + k]->SetPosition(195, 48, -50);
				}
			}
			//4R
			if (pt.x > 106 && pt.x < 906 && pt.y>368 && pt.y < 428)
			{
				my_room_num = 4;
				m_lobbyObjects[10 + 6 * i]->SetPosition(0, 10, -49);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[37 + k]->SetPosition(195, 12, -58);
				}
			}
			else
			{
				m_lobbyObjects[10 + 6 * i]->SetPosition(0, 0, -49);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[37 + k]->SetPosition(195, 2, -58);
				}
			}
			//5R
			if (pt.x > 106 && pt.x < 906 && pt.y>463 && pt.y < 523)
			{
				my_room_num = 5;
				m_lobbyObjects[11 + 6 * i]->SetPosition(0, -40, -57);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[43 + k]->SetPosition(195, -38, -66);
				}
			}
			else
			{
				m_lobbyObjects[11 + 6 * i]->SetPosition(0, -50, -57);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[43 + k]->SetPosition(195, -48, -66);
				}
			}
			//6R
			if (pt.x > 106 && pt.x < 906 && pt.y>561 && pt.y < 621)
			{
				my_room_num = 6;
				m_lobbyObjects[12 + 6 * i]->SetPosition(0, -90, -65);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[49 + k]->SetPosition(195, -88, -74);
				}
			}
			else
			{
				m_lobbyObjects[12 + 6 * i]->SetPosition(0, -100, -65);
				for (int k = 0; k < 6; ++k)
				{
					m_lobbyObjects[49 + k]->SetPosition(195, -98, -74);
				}
			}

		}
	}
	

}

void LobbyScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	m_pFramework->SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	//if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	//ConsolePrint("%d\n", m_pFramework->nBase_member[0]);
	if (m_lobbyObjects) {
		
		m_lobbyObjects[6]->Render(pd3dCommandList, pCamera); // 방 생성 
		
		// 화살표
		/*if (left_active == true)
		{
			m_lobbyObjects[4]->Render(pd3dCommandList, pCamera);
		}
		else
		{
			m_lobbyObjects[2]->Render(pd3dCommandList, pCamera);
		}
		if (right_active == true)
		{
			m_lobbyObjects[5]->Render(pd3dCommandList, pCamera);
		}
		else
		{
			m_lobbyObjects[3]->Render(pd3dCommandList, pCamera);
		}*/

		// 방 


		if (list_num == 1) {
			
			// 방에 입장한 인원 
			for (int i = 0; i < 6; ++i)
			{
				if (m_pFramework->nBase_member[0] == i+1)
				{
					m_lobbyObjects[19+i]->Render(pd3dCommandList, pCamera);
				}

				if (m_pFramework->nBase_member[1] == i + 1)
				{
					m_lobbyObjects[25 + i]->Render(pd3dCommandList, pCamera);
				}

				if (m_pFramework->nBase_member[2] == i + 1)
				{
					m_lobbyObjects[31 + i]->Render(pd3dCommandList, pCamera);
				}

				if (m_pFramework->nBase_member[3] == i + 1)
				{
					m_lobbyObjects[37 + i]->Render(pd3dCommandList, pCamera);
				}

				if (m_pFramework->nBase_member[4] == i + 1)
				{
					m_lobbyObjects[43 + i]->Render(pd3dCommandList, pCamera);
				}

				if (m_pFramework->nBase_member[5] == i + 1)
				{
					m_lobbyObjects[49 + i]->Render(pd3dCommandList, pCamera);
				}
			}
			if (m_pFramework->nBase_room[0] == 1)
				m_lobbyObjects[7]->Render(pd3dCommandList, pCamera);
			if (m_pFramework->nBase_room[1] == 2)
				m_lobbyObjects[8]->Render(pd3dCommandList, pCamera);
			if (m_pFramework->nBase_room[2] == 3)
				m_lobbyObjects[9]->Render(pd3dCommandList, pCamera);
			if (m_pFramework->nBase_room[3] == 4)
				m_lobbyObjects[10]->Render(pd3dCommandList, pCamera);
			if (m_pFramework->nBase_room[4] == 5)
				m_lobbyObjects[11]->Render(pd3dCommandList, pCamera);
			if (m_pFramework->nBase_room[5] == 6)
				m_lobbyObjects[12]->Render(pd3dCommandList, pCamera);

		}

		
		/*if (list_num == 2)
		{
			if (m_pFramework->room_num >= 7)
				m_lobbyObjects[13]->Render(pd3dCommandList, pCamera);
			if (m_pFramework->room_num >= 8)
				m_lobbyObjects[14]->Render(pd3dCommandList, pCamera);
			if (m_pFramework->room_num >= 9)
				m_lobbyObjects[15]->Render(pd3dCommandList, pCamera);
			if (m_pFramework->room_num >= 10)
				m_lobbyObjects[16]->Render(pd3dCommandList, pCamera);
			if (m_pFramework->room_num >= 11)
				m_lobbyObjects[17]->Render(pd3dCommandList, pCamera);
			if (m_pFramework->room_num >= 12)
				m_lobbyObjects[18]->Render(pd3dCommandList, pCamera);
		}*/


		
		m_lobbyObjects[1]->Render(pd3dCommandList, pCamera); // 방 목록 배경
		m_lobbyObjects[0]->Render(pd3dCommandList, pCamera); //로비 배경
	}
}

void LobbyScene::ProcessPacket(char * ptr)
{
	static bool first_time = true;

	switch (ptr[1])
	{
	case SC_Type::LoginOK:
	{
		//ConsolePrint("login\n");
		SCPacket_LoginOK *packet = reinterpret_cast<SCPacket_LoginOK *>(ptr);
		m_pFramework->myid = packet->id;
		ConsolePrint("LOGIN\n");
		break;
	}
	case SC_Type::CreateRoom:
	{

		SCPacket_CreateRoom *packet = reinterpret_cast<SCPacket_CreateRoom*>(ptr);
		m_pFramework->room_num = packet->room_num;
		//	m_lobbyObjects[room_num + 6]->n_member = packet->player_num;
		m_lobbyObjects[m_pFramework->room_num + 6]->room_name = packet->room_num;
		m_pFramework->nBase_room[m_pFramework->room_num - 1] = packet->room_num; // n번방
		m_pFramework->nBase_member[m_pFramework->room_num - 1] = packet->player_num; // n번방의 인원수
		if (room_enter == true)
		{
			//SendLobby_RoomChange(1);
			m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
			room_enter = false;
		}
		//ConsolePrint("생성후 룸정보%d, %d, %d\n", m_pFramework->room_num, m_pFramework->nBase_room[m_pFramework->room_num - 1], m_pFramework->nBase_room[m_pFramework->room_num - 1]);
		break;
	}
	case SC_Type::EnterRoom:
	{
		
		SCPacket_EnterRoom *packet = reinterpret_cast<SCPacket_EnterRoom*>(ptr);
		m_pFramework->room_num = packet->room_num;
	//	m_lobbyObjects[id_room_num + 6]->n_member = packet->player_num;
		m_pFramework->nBase_room[m_pFramework->room_num - 1] = packet->room_num; // n번방
		m_pFramework->nBase_member[m_pFramework->room_num - 1] = packet->player_num; // n번방의 인원수
		if (room_enter == true)
		{
			//SendLobby_RoomChange(my_room_num);
			m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
			room_enter = false;
		}
		//ConsolePrint("입장후 %d, %d, %d\n", m_pFramework->room_num, m_pFramework->nBase_room[m_pFramework->room_num - 1], m_pFramework->nBase_member[m_pFramework->room_num - 1]);
		//ConsolePrint("%d\n", packet->player_num);
		break;
	}
	case SC_Type::ExitRoom:
	{
		SCPacket_ExitRoom *packet = reinterpret_cast<SCPacket_ExitRoom*>(ptr);
		m_pFramework->room_num = packet->room_num;
		if (packet->player_num == 0)
		{
			m_pFramework->nBase_room[m_pFramework->room_num] = 0;  // 룸번호
			m_pFramework->nBase_member[m_pFramework->room_num] = packet->player_num;
		}
		else
		{
			m_pFramework->nBase_room[m_pFramework->room_num - 1] = packet->room_num;  // 룸번호
			m_pFramework->nBase_member[m_pFramework->room_num - 1] = packet->player_num; // 룸에 있는 인원
		}
		ConsolePrint("Room Number : %d\n", m_pFramework->room_num);
//		ConsolePrint("%d, %d, %d\n", room_num, m_pFramework->nBase_room[room_num - 1], m_pFramework->nBase_member[room_num - 1]);
		break;
	}
	
	case SC_Type::PutPlayer:
	{
		SCPacket_PutPlayer *my_packet = reinterpret_cast<SCPacket_PutPlayer *>(ptr);
	

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
	
	default:
		ConsolePrint("Lobby Scene - Unknown PACKET type [%d]\n", ptr[1]);
		break;
	}
}



void LobbyScene::SendRoomCreate()
{
	CSPacket_RoomCreate *roomPacket = reinterpret_cast<CSPacket_RoomCreate*>(m_pFramework->GetSendBuf());
	roomPacket->size = sizeof(CSPacket_RoomCreate);
	roomPacket->player_num = 1;
	roomPacket->room_num = m_pFramework->nBase_room[m_pFramework->room_num];  //  원래는room_num
	roomPacket->type = CS_Type::Room_Create;
	roomPacket->scene = nCurrScene;
	roomPacket->check = room_enter;
	m_pFramework->SendPacket(reinterpret_cast<char *>(roomPacket));
	
	//ConsolePrint("생성전 룸정보%d, %d\n", m_pFramework->room_num, m_pFramework->nBase_room[m_pFramework->room_num]);
}


void LobbyScene::SendEnterRoom()
{
	CSPacket_RoomEnter *roomPacket = reinterpret_cast<CSPacket_RoomEnter*>(m_pFramework->GetSendBuf());
	roomPacket->size = sizeof(CSPacket_RoomEnter);
	//roomPacket->player_num = m_lobbyObjects[6 + my_room_num]->n_member;
	//roomPacket->room_num = my_room_num;   // 룸넘에서 마이 룸넘으로 교체
	//roomPacket->room_num = m_pFramework->nBase_room[m_pFramework->room_num - 1];
	roomPacket->room_num = my_room_num;
	//roomPacket->player_num = m_pFramework->nBase_member[m_pFramework->room_num - 1];
	roomPacket->player_num = m_pFramework->nBase_member[my_room_num - 1];
	roomPacket->type = CS_Type::Room_Enter;
	roomPacket->scene = nCurrScene;
	roomPacket->check = room_enter;
	m_pFramework->SendPacket(reinterpret_cast<char *>(roomPacket));
	//ConsolePrint("입장전 룸정보%d, %d, %d\n", m_pFramework->room_num, m_pFramework->nBase_room[m_pFramework->room_num - 1], m_pFramework->nBase_member[m_pFramework->room_num - 1]);
	
}

void LobbyScene::SendLobby_RoomChange(unsigned short room_n)
{
	CSPacket_SceneChange_L_R *myPacket = reinterpret_cast<CSPacket_SceneChange_L_R*>(m_pFramework->GetSendBuf());
	myPacket->size = sizeof(CSPacket_SceneChange_L_R);
	myPacket->type = CS_Type::Scene_Change_L_R;
	myPacket->player_num = m_pFramework->nBase_member[m_pFramework->room_num - 1]; // 현재 인원수
	myPacket->room_num = m_pFramework->nBase_room[m_pFramework->room_num - 1];  // 방 번호  생성할때는 1 입장할때는 마이룸넘
	myPacket->scene = BaseScene::SceneTag::Lobby;
	myPacket->check = room_enter;
	m_pFramework->SendPacket(reinterpret_cast<char *>(myPacket));
	
}
