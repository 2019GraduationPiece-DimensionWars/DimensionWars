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
#include "Object104_DummyPlayer.h"
TitleScene::TitleScene(SceneTag tag, RuntimeFrameWork * pFramework) : BaseScene(tag, pFramework)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	//m_pFramework->m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	//CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 2, 45);

	
	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature);
	m_pFramework->GetResource()->AllModelLoad(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature);

	BuildLightsAndMaterials();

	


	m_nObjects4 = 1;
	m_titleObjects = new BaseObject*[m_nObjects4];
	//

	
	Object104_DummyPlayer *pPlayer = new Object104_DummyPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	m_pPlayer = pPlayer;


	Texture * titleImage = new Texture(1, RESOURCE_TEXTURE2D, 0);
	titleImage->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Titleimage.dds", 0);

	TextureRectangleShader *pTextureShader = new TextureRectangleShader();
	pTextureShader->CreateShader(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature);
	pTextureShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, titleImage, 15, true);

	Material *titleMaterial[1];
	titleMaterial[0] = new Material(1);
	titleMaterial[0]->SetTexture(titleImage);
	titleMaterial[0]->SetShader(pTextureShader);

	
	
	TextureRectObject *titleImageObject = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 680, 480);
	m_titleObjects[0] = titleImageObject;
	// 카메라를 위해 생성  렌더하지 않음
	m_titleObjects[0]->SetMaterial(0, titleMaterial[0]);
	m_titleObjects[0]->SetPosition(0, -25, 0);
	
	
	
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	

}

void TitleScene::ReleaseObjects()
{
	if (m_pFramework->m_pGraphicsRootSignature) m_pFramework->m_pGraphicsRootSignature->Release();

	
	ReleaseShaderVariables();
}

void TitleScene::ReleaseUploadBuffers()
{
	
	if (m_titleObjects) if (m_titleObjects[0]) m_titleObjects[0]->ReleaseUploadBuffers();
}

bool TitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		if (!network_init) {
			m_pFramework->NetworkInitialize();
			network_init = true;
		}
		m_pFramework->ChangeScene(BaseScene::SceneTag::Lobby);
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
	//printf("%f, %f, %f\n", m_pPlayer->GetLookVector().x, m_pPlayer->GetLookVector().y, m_pPlayer->GetLookVector().z);


}

void TitleScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	m_pFramework->SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);


	if (m_titleObjects) if (m_titleObjects[0]) m_titleObjects[0]->Render(pd3dCommandList, pCamera);
	
}
