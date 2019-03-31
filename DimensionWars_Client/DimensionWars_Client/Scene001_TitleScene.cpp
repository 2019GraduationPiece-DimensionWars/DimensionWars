#include "stdafx.h"
#include "Scene001_TitleScene.h"
#include "Object003_SkyBox.h"
#include "Camera000_BaseCamera.h"
#include "Object101_GrimReaperPlayer.h"
#include "Material.h"


TitleScene::TitleScene(SceneTag tag, RuntimeFrameWork * pFramework) : BaseScene(tag, pFramework)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 45); //SuperCobra(17), Gunship(2), Player:Mi24(1), Angrybot()

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildLightsAndMaterials();

	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
}

void TitleScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_pSkyBox) delete m_pSkyBox;

	ReleaseShaderVariables();
}

void TitleScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
}

void TitleScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);

	if (m_pPlayer) m_pPlayer->Render(pd3dCommandList, pCamera);
}

bool TitleScene::ProcessInput(UCHAR * pKeysBuffer)
{
	DWORD dwDirection = 0;
	if (pKeysBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeysBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeysBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeysBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
	if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
	if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeysBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		if (dwDirection) m_pPlayer->Move(dwDirection, 12.25f, true);
	}

	return false;
}
