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

	int room_count = 27;
	m_nObjects2 = 9;
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

	for (int i = 7; i < 8; ++i)
	{
		TextureRectObject *lobbyImageObject7 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, L"Texture/Lobby/room_list.dds", 450, 28);
		m_lobbyObjects[i] = lobbyImageObject7;
		m_lobbyObjects[i]->SetPosition(0, 150, -25);
	}
	TextureRectObject *lobbyImageObject8 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, L"Texture/Lobby/room_list.dds", 450, 28);
	m_lobbyObjects[8] = lobbyImageObject8;
	m_lobbyObjects[8]->SetPosition(0, 120, -25);
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
		//if (pt.x > 380 && pt.x < 445 && pt.y>650 && pt.y < 680)
		//{
		//	a = 1;
		//}
		////우측 화살표
		//if (pt.x > 580 && pt.x < 640 && pt.y>650 && pt.y < 680)
		//{
		//	a = 0;
		//}
		// 방 생성
		if (pt.x > 824 && pt.x < 950 && pt.y>665 && pt.y < 725)
		{
			m_pFramework->ChangeScene(BaseScene::SceneTag::Room);
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
}

void LobbyScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	m_pFramework->SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	//if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);

	if (m_lobbyObjects) {
		m_lobbyObjects[0]->Render(pd3dCommandList, pCamera);
		m_lobbyObjects[1]->Render(pd3dCommandList, pCamera);
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

		for (int i = 6; i < m_nObjects2; ++i)
		{
			
			m_lobbyObjects[i]->Render(pd3dCommandList, pCamera);
		}
		
		
	}
}
