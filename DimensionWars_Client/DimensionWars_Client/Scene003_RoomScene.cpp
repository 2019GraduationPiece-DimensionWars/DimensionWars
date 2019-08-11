#include "stdafx.h"
#include "Scene003_RoomScene.h"
#include "Material.h"
#include "Object006_TextureRectObject.h"
#include "Camera000_BaseCamera.h"
#include "RuntimeFrameWork.h"
#include "Texture.h"
#include "Shader005_TextureRectangleShader.h"
RoomScene::RoomScene()
{
}


RoomScene::~RoomScene()
{
}

void RoomScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_nObjects3 = 15; 
	m_roomObjects = new BaseObject*[m_nObjects3];
	//
	nCurrScene = 2;
	Texture *roomImage[room_texture];

	roomImage[0] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage[0]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/Room_bg.dds", 0);

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
	roomImage[12]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Room/Random_select.dds", 0);



	


	TextureRectangleShader *pTextureShader = new TextureRectangleShader();
	pTextureShader->CreateShader(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature);
	pTextureShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	
	for (int i = 0; i < room_texture; ++i)
	{
		CreateShaderResourceViews(pd3dDevice, pd3dCommandList, roomImage[i], 15, true);
	}


	Material *roomMaterial[room_texture];

	//로비에있는 화살표와 로비배경 가져다 쓰기 
	
	for (int i = 0; i < room_texture; ++i)
	{
		roomMaterial[i] = new Material(1);
		roomMaterial[i]->SetTexture(roomImage[i]);
		roomMaterial[i]->SetShader(pTextureShader);
	}
	

	// 룸배경
	TextureRectObject *roomImageObject = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 600, 450);
	m_roomObjects[0] = roomImageObject;
	m_roomObjects[0]->SetMaterial(0, roomMaterial[0]);
	m_roomObjects[0]->SetPosition(0, 0, 8);
	// 내 캐릭터 선택배경
	TextureRectObject *roomImageObject1 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 90, 180);
	m_roomObjects[1] = roomImageObject1;
	m_roomObjects[1]->SetMaterial(0, roomMaterial[1]);
	m_roomObjects[1]->SetPosition(-230, 60, 6);
	// 상대 캐릭터 배경
	TextureRectObject *roomImageObject2 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 450, 300);
	m_roomObjects[2] = roomImageObject2;
	m_roomObjects[2]->SetMaterial(0, roomMaterial[2]);
	m_roomObjects[2]->SetPosition(55, 55, 6);
	// 왼쪽 비활성
	TextureRectObject *roomImageObject3 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 30, 20);
	m_roomObjects[3] = roomImageObject3;
	m_roomObjects[3]->SetMaterial(0, roomMaterial[3]);
	m_roomObjects[3]->SetPosition(-260, -55, -2);
	// 오른쪽 비활성
	TextureRectObject *roomImageObject4 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 30, 20);
	m_roomObjects[4] = roomImageObject4;
	m_roomObjects[4]->SetMaterial(0, roomMaterial[4]);
	m_roomObjects[4]->SetPosition(-206, -55, -2);
	// 왼쪽 활성 
	TextureRectObject *roomImageObject5 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 30, 20);
	m_roomObjects[5] = roomImageObject5;
	m_roomObjects[5]->SetMaterial(0, roomMaterial[5]);
	m_roomObjects[5]->SetPosition(-260, -55, -2);
	// 오른쪽 활성
	TextureRectObject *roomImageObject6 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 30, 20);
	m_roomObjects[6] = roomImageObject6;
	m_roomObjects[6]->SetMaterial(0, roomMaterial[6]);
	m_roomObjects[6]->SetPosition(-206, -55, -2);
	// 방 나가기
	TextureRectObject *roomImageObject7 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 80, 40);
	m_roomObjects[7] = roomImageObject7;
	m_roomObjects[7]->SetMaterial(0, roomMaterial[7]);
	m_roomObjects[7]->SetPosition(-240, -190, -25);
	// 게임시작
	TextureRectObject *roomImageObject8 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 80, 40);
	m_roomObjects[8] = roomImageObject8;
	m_roomObjects[8]->SetMaterial(0, roomMaterial[8]);
	m_roomObjects[8]->SetPosition(240, -190, -25);
	//상대 플레이어
	TextureRectObject *roomImageObject9 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 130, 130);
	m_roomObjects[9] = roomImageObject9;
	m_roomObjects[9]->SetMaterial(0, roomMaterial[9]);
	m_roomObjects[9]->SetPosition(-80, 100, 5);

	TextureRectObject *roomImageObject10 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 130, 130);
	m_roomObjects[10] = roomImageObject10;
	m_roomObjects[10]->SetMaterial(0, roomMaterial[10]);
	m_roomObjects[10]->SetPosition(60, 100, 5);

	TextureRectObject *roomImageObject11 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 130, 130);
	m_roomObjects[11] = roomImageObject11;
	m_roomObjects[11]->SetMaterial(0, roomMaterial[11]);
	m_roomObjects[11]->SetPosition(200, 100, 5);

	TextureRectObject *roomImageObject12 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 130, 130);
	m_roomObjects[12] = roomImageObject12;
	m_roomObjects[12]->SetMaterial(0, roomMaterial[12]);
	m_roomObjects[12]->SetPosition(-80, -20, -10);

	TextureRectObject *roomImageObject13 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 130, 130);
	m_roomObjects[13] = roomImageObject13;
	m_roomObjects[13]->SetMaterial(0, roomMaterial[12]);
	m_roomObjects[13]->SetPosition(60, -20, -10);

	TextureRectObject *roomImageObject14 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 130, 130);
	m_roomObjects[14] = roomImageObject14;
	m_roomObjects[14]->SetMaterial(0, roomMaterial[12]);
	m_roomObjects[14]->SetPosition(200, -20, -10);
	

	for (int i = 0; i < m_nObjects3; ++i)
	{
		m_roomObjects[i]->Rotate(8, 0, 0);
	}

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
		//printf("%d, %d\n", pt.x, pt.y);
		if (pt.x > 55 && pt.x < 180 && pt.y>666 && pt.y < 726)
		{
			room_exit = true;
			SendRoomExit();
			//SendRomm_LobbyChange();
			//SendSceneChange();
			//m_pFramework->ChangeScene(BaseScene::SceneTag::Lobby);
		}
		if (pt.x > 841 && pt.x < 966 && pt.y>666 && pt.y < 726)
		{
			//m_pPlayer = m_pFramework->arrScene[BaseScene::SceneTag::Game]->m_pPlayer;
			//m_pFramework->arrScene[BaseScene::SceneTag::Game]->BuildObjects(m_pFramework->m_pDevice, m_pFramework->m_pCommandList);
			m_pFramework->ChangeScene(BaseScene::SceneTag::Game);
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
	
	//printf("룸%d, 입장한 수%d\n",m_pFramework->nBase_room[0], m_pFramework->nBase_member[0]);
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

	//printf("%d, %d, %d\n", m_pFramework->room_num, m_pFramework->nBase_room[m_pFramework->room_num - 1], m_pFramework->nBase_member[m_pFramework->room_num - 1]);
}

void RoomScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	m_pFramework->SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	

	if (m_roomObjects)
	{
		m_roomObjects[0]->Render(pd3dCommandList, pCamera);
		m_roomObjects[1]->Render(pd3dCommandList, pCamera);
		m_roomObjects[2]->Render(pd3dCommandList, pCamera);
		m_roomObjects[7]->Render(pd3dCommandList, pCamera);
		m_roomObjects[8]->Render(pd3dCommandList, pCamera);

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
		
	}
}

void RoomScene::ProcessPacket(char * ptr)
{
	static bool first_time = true;

	switch (ptr[1])
	{
	case SC_Type::CreateRoom:
	{

		SCPacket_CreateRoom *packet = reinterpret_cast<SCPacket_CreateRoom*>(ptr);
		//room_num = packet->room_num;
		//	m_lobbyObjects[room_num + 6]->n_member = packet->player_num;
		//m_pFramework->nBase_room[m_pFramework->room_num - 1] = packet->room_num; // n번방
		//m_pFramework->nBase_member[m_pFramework->room_num - 1] = packet->player_num; // n번방의 인원수

		
		break;
	}
	case SC_Type::EnterRoom:
	{

		SCPacket_EnterRoom *packet = reinterpret_cast<SCPacket_EnterRoom*>(ptr);
		//room_num = packet->room_num;
		//	m_lobbyObjects[id_room_num + 6]->n_member = packet->player_num;
		//m_pFramework->nBase_room[m_pFramework->room_num - 1] = packet->room_num; // n번방
		//m_pFramework->nBase_member[m_pFramework->room_num - 1] = packet->player_num; // n번방의 인원수
		
		break;
	}
	case SC_Type::ExitRoom:
	{
		SCPacket_ExitRoom *packet = reinterpret_cast<SCPacket_ExitRoom*>(ptr);
		m_pFramework->room_num = packet->room_num;
		if (packet->player_num == 0)
		{
			m_pFramework->nBase_room[m_pFramework->room_num - 1] = 0;  // 룸번호
			m_pFramework->nBase_member[m_pFramework->room_num - 1] = packet->player_num; // 룸에 있는 인원
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
			m_pFramework->ChangeScene(BaseScene::SceneTag::Lobby);
			//printf("룸%d\n", m_pFramework->nBase_member[room_num - 1]);
			
			room_exit = false;
		}
		//printf("%d, %d, %d\n", room_num, m_pFramework->nBase_room[room_num - 1], m_pFramework->nBase_member[room_num - 1]);
		//printf("받음\n");
		break;
	}

	// 로비에서 보낸것을 룸에서 받고 처리
	case SC_Type::ChangeScene_L_R:
	{
		SCPacket_ChangeScene_L_R *packet = reinterpret_cast<SCPacket_ChangeScene_L_R*>(ptr);
		//room_num = packet->room_num;
		//m_pFramework->nBase_room[room_num - 1] = packet->room_num;  // 룸번호와
		//m_pFramework->nBase_member[room_num - 1] = packet->player_num; // 룸 에있는 인원
		//printf("받음\n");
		break;
	}
	
	case SC_Type::ChangeScene_R_L:
	{
		SCPacket_ChangeScene_R_L *packet = reinterpret_cast<SCPacket_ChangeScene_R_L*>(ptr);
		//room_num = packet->room_num;
		//m_pFramework->nBase_room[room_num - 1] = packet->room_num;
		//m_pFramework->nBase_member[room_num - 1] = packet->player_num;
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
