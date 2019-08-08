#include "stdafx.h"
#include "Scene002_LobbyScene.h"
#include "Material.h"
#include "Object006_TextureRectObject.h"
#include "Camera000_BaseCamera.h"
#include "RuntimeFrameWork.h"
#include "Object100_BasePlayer.h"
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

	m_nObjects2 = 19; //max 42
	m_lobbyObjects = new BaseObject*[m_nObjects2];
	//
	
	// 로비배경
	TextureRectObject *lobbyImageObject = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, L"Texture/Lobby/lobby_bg.dds", 600, 450);
	m_lobbyObjects[0] = lobbyImageObject;
	m_lobbyObjects[0]->SetPosition(0, 0, 2);
	// 방 배경
	TextureRectObject *lobbyImageObject1 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, L"Texture/Lobby/list_bg.dds", 550, 350);
	m_lobbyObjects[1] = lobbyImageObject1;
	m_lobbyObjects[1]->SetPosition(0, 25, 1);
	// 왼쪽 비활성
	TextureRectObject *lobbyImageObject2 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, L"Texture/Lobby/left_not_act.dds", 40, 20);
	m_lobbyObjects[2] = lobbyImageObject2;
	m_lobbyObjects[2]->SetPosition(-60, -170, -22);
	// 오른쪽 비활성
	TextureRectObject *lobbyImageObject3 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, L"Texture/Lobby/right_not_act.dds", 40, 20);
	m_lobbyObjects[3] = lobbyImageObject3;
	m_lobbyObjects[3]->SetPosition(60, -170, -22);
	// 왼쪽 활성
	TextureRectObject *lobbyImageObject4 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, L"Texture/Lobby/left_act.dds", 40, 20);
	m_lobbyObjects[4] = lobbyImageObject4;
	m_lobbyObjects[4]->SetPosition(-60, -170, -22);
	// 오른쪽 활성
	TextureRectObject *lobbyImageObject5 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, L"Texture/Lobby/right_act.dds", 40, 20);
	m_lobbyObjects[5] = lobbyImageObject5;
	m_lobbyObjects[5]->SetPosition(60, -170, -22);
	// 방 생성
	TextureRectObject *lobbyImageObject6 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, L"Texture/Lobby/room_ct.dds", 80, 40);
	m_lobbyObjects[6] = lobbyImageObject6;
	m_lobbyObjects[6]->SetPosition(230, -190, -25);

	for (int i = 7; i < m_nObjects2; ++i)
	{
		TextureRectObject *lobbyImageObject7 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, L"Texture/Lobby/room_list.dds", 450, 40);
		m_lobbyObjects[i] = lobbyImageObject7;
		//m_lobbyObjects[i]->SetPosition(0, 150, -25);
	}

	for (int i = 0; i < 6; ++i)
	{
		m_lobbyObjects[7+i]->SetPosition(0, 150 - 50 * i, -25 - 8 * i);;
	}
	for (int i = 0; i < 6; ++i)
	{
		m_lobbyObjects[13 + i]->SetPosition(0, 150 - 50 * i, -25 - 8 * i);;
	}
	
	for (int i = 0; i < m_nObjects2; ++i)
	{
		m_lobbyObjects[i]->Rotate(8, 0, 0);
	}
	
	
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void LobbyScene::ReleaseObjects()
{
}

void LobbyScene::ReleaseUploadBuffers()
{
	if (m_lobbyObjects) {
		for (int i = 0; i < m_nObjects2; ++i) {
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
		printf("%d, %d\n", pt.x, pt.y);
		//좌측 화살표
		if (pt.x > 380 && pt.x < 445 && pt.y>650 && pt.y < 680)
		{
			list_num = 1;
		}
		//우측 화살표
		if (pt.x > 580 && pt.x < 640 && pt.y>650 && pt.y < 680)
		{
			list_num = 2;
		}
		// 방 생성
		if (pt.x > 824 && pt.x < 950 && pt.y>665 && pt.y < 725)
		{
			room_num += 1;
			if (room_num > 6)
			{
				list_num = 2;
			}
			
			if (room_num <= 12) {
				SendRoomCreate();
			}
			//m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
		}
		// 방 리스트 클릭
		if (list_num == 1) {
			if (pt.x > 97 && pt.x < 927 && pt.y>80 && pt.y < 144)
			{
				if (m_lobbyObjects[7]->room_name == my_room_num) {
					m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
				}
			}
			if (pt.x > 97 && pt.x < 927 && pt.y>175 && pt.y < 235)
			{
				if (m_lobbyObjects[8]->room_name == my_room_num) {
					m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
				}
			}
			if (pt.x > 97 && pt.x < 927 && pt.y>266 && pt.y < 336)
			{
				if (m_lobbyObjects[9]->room_name == my_room_num) {
					m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
				}
			}
			if (pt.x > 97 && pt.x < 927 && pt.y>363 && pt.y < 425)
			{
				if (m_lobbyObjects[10]->room_name == my_room_num) {
					m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
				}
			}
			if (pt.x > 97 && pt.x < 927 && pt.y>459 && pt.y < 522)
			{
				if (m_lobbyObjects[11]->room_name == my_room_num) {
					m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
				}
			}
			if (pt.x > 97 && pt.x < 927 && pt.y>555 && pt.y < 616)
			{
				if (m_lobbyObjects[12]->room_name == my_room_num) {
					m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
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
	if (pt.x > 380 && pt.x < 445 && pt.y>650 && pt.y < 680)
		left_active = true;
	else
		left_active = false;
	//우측 화살표
	if (pt.x > 580 && pt.x < 640 && pt.y>650 && pt.y < 680)
		right_active = true;
	else
		right_active = false;
	// 방 생성
	if (pt.x > 824 && pt.x < 950 && pt.y>665 && pt.y < 725)
		m_lobbyObjects[6]->SetPosition(230, -180, -25);
	
	else
		m_lobbyObjects[6]->SetPosition(230, -190, -25);

	// 룸 입장
	if (list_num == 1) {
		if (pt.x > 97 && pt.x < 927 && pt.y>80 && pt.y < 144)
		{
			my_room_num = 1;
			m_lobbyObjects[7]->SetPosition(0, 160, -25);
		}
		else
		{
			m_lobbyObjects[7]->SetPosition(0, 150, -25);
		}
		if (pt.x > 97 && pt.x < 927 && pt.y>175 && pt.y < 235)
		{
			my_room_num = 2;
			m_lobbyObjects[8]->SetPosition(0, 110, -33);
		}
		else
		{
			m_lobbyObjects[8]->SetPosition(0, 100, -33);
		}
		if (pt.x > 97 && pt.x < 927 && pt.y>266 && pt.y < 336)
		{
			my_room_num = 3;
			m_lobbyObjects[9]->SetPosition(0, 60, -41);
		}
		else
		{
			m_lobbyObjects[9]->SetPosition(0, 50, -41);
		}
		if (pt.x > 97 && pt.x < 927 && pt.y>363 && pt.y < 425)
		{
			my_room_num = 4;
			m_lobbyObjects[10]->SetPosition(0, 10, -49);
		}
		else
		{
			m_lobbyObjects[10]->SetPosition(0, 0, -49);
		}
		if (pt.x > 97 && pt.x < 927 && pt.y>459 && pt.y < 522)
		{
			my_room_num = 5;
			m_lobbyObjects[11]->SetPosition(0, -40, -57);
		}
		else
		{
			m_lobbyObjects[11]->SetPosition(0, -50, -57);
		}
		if (pt.x > 97 && pt.x < 927 && pt.y>555 && pt.y < 616)
		{
			my_room_num = 6;
			m_lobbyObjects[12]->SetPosition(0, -90, -65);
		}
		else
		{
			m_lobbyObjects[12]->SetPosition(0, -100, -65);
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

	if (m_lobbyObjects) {
		m_lobbyObjects[0]->Render(pd3dCommandList, pCamera); //로비 배경
		m_lobbyObjects[1]->Render(pd3dCommandList, pCamera); // 방 목록 배경
		m_lobbyObjects[6]->Render(pd3dCommandList, pCamera); // 방 생성 
		// 화살표
		if (left_active == true)
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
		}

		// 방 


		if (list_num == 1) {
			if (room_num >= 1)
				m_lobbyObjects[7]->Render(pd3dCommandList, pCamera);
			if (room_num >= 2)
				m_lobbyObjects[8]->Render(pd3dCommandList, pCamera);
			if (room_num >= 3)
				m_lobbyObjects[9]->Render(pd3dCommandList, pCamera);
			if (room_num >= 4)
				m_lobbyObjects[10]->Render(pd3dCommandList, pCamera);
			if (room_num >= 5)
				m_lobbyObjects[11]->Render(pd3dCommandList, pCamera);
			if (room_num >= 6)
				m_lobbyObjects[12]->Render(pd3dCommandList, pCamera);
		}

		
		if (list_num == 2)
		{
			if (room_num >= 7)
				m_lobbyObjects[13]->Render(pd3dCommandList, pCamera);
			if (room_num >= 8)
				m_lobbyObjects[14]->Render(pd3dCommandList, pCamera);
			if (room_num >= 9)
				m_lobbyObjects[15]->Render(pd3dCommandList, pCamera);
			if (room_num >= 10)
				m_lobbyObjects[16]->Render(pd3dCommandList, pCamera);
			if (room_num >= 11)
				m_lobbyObjects[17]->Render(pd3dCommandList, pCamera);
			if (room_num >= 12)
				m_lobbyObjects[18]->Render(pd3dCommandList, pCamera);
		}



	}
}

void LobbyScene::ProcessPacket(char * ptr)
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
	case SC_Type::CreateRoom:
	{

		SCPacket_CreateRoom *packet = reinterpret_cast<SCPacket_CreateRoom*>(ptr);
		room_num = packet->room_num;
		player_num = packet->player_num;
		m_lobbyObjects[room_num + 6]->room_name = packet->room_num;
		break;
	}
	
	default:
#ifdef USE_CONSOLE_WINDOW
		printf("Unknown PACKET type [%d]\n", ptr[1]);
#endif
		break;
	}
}



void LobbyScene::SendRoomCreate()
{
	CSPacket_RoomCreate *roomPacket = reinterpret_cast<CSPacket_RoomCreate*>(m_pFramework->GetSendBuf());
	roomPacket->size = sizeof(CSPacket_RoomCreate);
	roomPacket->player_num = player_num;
	roomPacket->room_num = room_num;
	roomPacket->type = CS_Type::Room_Create;

	m_pFramework->SendPacket(reinterpret_cast<char *>(roomPacket));
	
}
