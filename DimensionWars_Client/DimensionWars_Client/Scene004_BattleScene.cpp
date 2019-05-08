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

	XMFLOAT3 xmf3Scale(8.0f, 2.0f, 8.0f);
	XMFLOAT4 xmf4Color(0.1f, 0.1f, 0.1f, 0.5f);
	//m_pTerrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, _T("Texture/HeightMap.raw"), 257, 257, xmf3Scale, xmf4Color);

	 //GrimReaperPlayer *pPlayer = new GrimReaperPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain);
	GamblerPlayer *pPlayer = new GamblerPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain);
	// ElfArcherPlayer *pPlayer = new ElfArcherPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain);
	m_pPlayer = pPlayer;

	m_nCubeObjects = 50;
	m_ppCubeObjects = new DiffuseCubeObject*[m_nCubeObjects];
	for (int i = 0; i < m_nCubeObjects; ++i) {
#ifdef USE_CONSOLE_WINDOW
		printf("Cube [%d] %.2f -  Pos : (%.2f, %.2f, %.2f) / Rot : (%.2f, %.2f, %.2f)\n", i, cubeSize[i], cubePos[i].x, cubePos[i].y, cubePos[i].z, cubeRot[i].x, cubeRot[i].y, cubeRot[i].z);
#endif
		if (i < 5) cubeSize[i] = 300.0f;
		else if (i < 10) cubeSize[i] = 400.0f;
		else if (i < 20) cubeSize[i] = 500.0f;
		else if (i < 30) cubeSize[i] = 600.0f;
		else if (i < 50) cubeSize[i] = 700.0f;
		m_ppCubeObjects[i] = new DiffuseCubeObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, cubeSize[i]);
		m_ppCubeObjects[i]->SetPosition(cubePos[i].x, cubePos[i].y, cubePos[i].z);
		m_ppCubeObjects[i]->Rotate(cubeRot[i].x, cubeRot[i].y, cubeRot[i].z);
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
}

void BattleScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);

	if (m_pPlayer) m_pPlayer->Render(pd3dCommandList, pCamera);

	for (unsigned int i = 0; i < m_nCubeObjects; ++i)
		if (m_ppCubeObjects && m_ppCubeObjects[i])
			m_ppCubeObjects[i]->Render(pd3dCommandList, pCamera);

	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
}

void BattleScene::BuildCube()
{
	if (!isBuilded) {
		for (int i = 0; i < m_nCubeObjects; ++i) {
			{
#ifdef USE_CONSOLE_WINDOW
				printf("UP Cube [%d] %.2f -  Pos : (%.2f, %.2f, %.2f) / Rot : (%.2f, %.2f, %.2f)\n", i, cubeSize[i], cubePos[i].x, cubePos[i].y, cubePos[i].z, cubeRot[i].x, cubeRot[i].y, cubeRot[i].z);
#endif
				m_ppCubeObjects[i]->SetPosition(cubePos[i].x, cubePos[i].y, cubePos[i].z);
				m_ppCubeObjects[i]->Rotate(cubeRot[i].x, cubeRot[i].y, cubeRot[i].z);
			}
		}
	}
}
