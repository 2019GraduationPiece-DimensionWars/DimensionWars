#include "stdafx.h"
#include "Scene003_RoomScene.h"


RoomScene::RoomScene()
{
}


RoomScene::~RoomScene()
{
}

void RoomScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void RoomScene::ReleaseObjects()
{
}

void RoomScene::ReleaseUploadBuffers()
{
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
}
