#include "stdafx.h"
#include "Scene004_BattleScene.h"


BattleScene::BattleScene()
{
}


BattleScene::~BattleScene()
{
}

void BattleScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void BattleScene::ReleaseObjects()
{
}

void BattleScene::ReleaseUploadBuffers()
{
}

bool BattleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool BattleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool BattleScene::ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed)
{
	return false;
}

void BattleScene::AnimateObjects(float fTimeElapsed)
{
}

void BattleScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
}
