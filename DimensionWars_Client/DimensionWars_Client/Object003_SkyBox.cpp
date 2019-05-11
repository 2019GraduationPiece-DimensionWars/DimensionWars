#include "stdafx.h"
#include "Object003_SkyBox.h"
#include "Mesh004_SkyBoxMesh.h"
#include "Shader001_SkyBoxShader.h"
#include "Texture.h"
#include "Material.h"
#include "Scene000_BaseScene.h"
#include "Camera000_BaseCamera.h"

SkyBox::SkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature) : BaseObject(1)
{
	SkyBoxMesh *pSkyBoxMesh = new SkyBoxMesh(pd3dDevice, pd3dCommandList, 6000.0f, 3000.0f, 6000.0f);
	SetMesh(pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Texture *pSkyBoxTexture = new Texture(1, RESOURCE_TEXTURE_CUBE, 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/SkyBox/SkyBox_1.dds", 0);

	SkyBoxShader *pSkyBoxShader = new SkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	BaseScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 10, false);

	Material *pSkyBoxMaterial = new Material(1);
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(0, pSkyBoxMaterial);
}

SkyBox::~SkyBox()
{
}

void SkyBox::Render(ID3D12GraphicsCommandList *pd3dCommandList, BaseCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	BaseObject::Render(pd3dCommandList, pCamera);
}