#include "stdafx.h"
#include "Scene003_RoomScene.h"
#include "Material.h"
#include "Object006_TextureRectObject.h"
#include "Camera000_BaseCamera.h"
#include "RuntimeFrameWork.h"
#include "Texture.h"
RoomScene::RoomScene()
{
}


RoomScene::~RoomScene()
{
}

void RoomScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	//m_pFramework->m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 2, 45);


	m_nObjects = 1;
	m_roomObject = new BaseObject*[m_nObjects];
	//
	Texture *roomImage;
	roomImage = new Texture(1, RESOURCE_TEXTURE2D, 0);
	roomImage->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Roomimage.dds", 0);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, roomImage, 15, true);
	TextureRectObject *RoomImageObject = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 500, 500);
	m_roomObject[0] = RoomImageObject;



	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void RoomScene::ReleaseObjects()
{
}

void RoomScene::ReleaseUploadBuffers()
{
	if (m_roomObject) if (m_roomObject[0]) m_roomObject[0]->ReleaseUploadBuffers();
}

bool RoomScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
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
}

void RoomScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	m_pFramework->SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	//if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);

	if (m_roomObject) if (m_roomObject[0]) m_roomObject[0]->Render(pd3dCommandList, pCamera);
}
