#include "stdafx.h"
#include "Scene001_TitleScene.h"
#include "Camera000_BaseCamera.h"
#include "Mesh005_TextureRectangleMesh.h"
#include "Object003_SkyBox.h"
#include "Object006_TextureRectObject.h"
#include "Shader005_TextureRectangleShader.h"
#include "Shader002_TextureVertexRectShader.h"
#include "Object101_GrimReaperPlayer.h"
#include "Material.h"
#include "Texture.h"
#include "Object008_HeightmapTerrain.h"
#include "RuntimeFrameWork.h"
#include "ResourceManager.h"
#include "Object102_GamblerPlayer.h"
TitleScene::TitleScene(SceneTag tag, RuntimeFrameWork * pFramework) : BaseScene(tag, pFramework)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 2, 45);

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pFramework->GetResource()->AllModelLoad(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	BuildLightsAndMaterials();

	

	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	m_nObjects = 1;
	m_titleObjects = new BaseObject*[m_nObjects];
	//
	GamblerPlayer *pPlayer = new GamblerPlayer(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	m_pPlayer = pPlayer;

	//m_pPlayer->GetCamera()->SetOffset(XMFLOAT3(0, 0, 0));
	//m_pPlayer->GetCamera()->SetPosition(XMFLOAT3(0, 0, 0));

	TextureRectObject *titleImageObject = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, L"Texture/Titleimage.dds",680,480);
	m_titleObjects[0] = titleImageObject;
	// 카메라를 위해 생성  렌더하지 않음
	
	m_titleObjects[0]->SetPosition(0, -25, 0);
	
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	

}

void TitleScene::ReleaseObjects()
{
	if (m_pGraphicsRootSignature) m_pGraphicsRootSignature->Release();

	if (m_pSkyBox) delete m_pSkyBox;

	ReleaseShaderVariables();
}

void TitleScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();

	if (m_titleObjects) if (m_titleObjects[0]) m_titleObjects[0]->ReleaseUploadBuffers();
}

bool TitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool TitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool TitleScene::ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed)
{
	return false;
}

void TitleScene::AnimateObjects(float fTimeElapsed)
{
}

void TitleScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	//if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);

	if (m_titleObjects) if (m_titleObjects[0]) m_titleObjects[0]->Render(pd3dCommandList, pCamera);
	
}
