#include "stdafx.h"
#include "Object014_ScreenTextureObject.h"
#include "Mesh008_ScreenTextureMesh.h"


ScreenTextureObject::ScreenTextureObject(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, float left, float top, float right, float bottom, unsigned int nMaterials) : BaseObject(nMaterials)
{
	ScreenTextureMesh *pHUDMesh = new ScreenTextureMesh(pd3dDevice, pd3dCommandList, left, top, right, bottom);
	SetMesh(pHUDMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//Texture * titleImage = new Texture(1, RESOURCE_TEXTURE2D, 0);
	//titleImage->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pTextureFileName, 0);

	//TextureRectangleShader *pTextureShader = new TextureRectangleShader();
	//pTextureShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//pTextureShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//
	////BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 15, true);
	//
	//Material * titleMaterial = new Material(1);
	////titleMaterial->SetTexture(pTexture);
	//titleMaterial->SetShader(pTextureShader);

	//SetMaterial(0, titleMaterial);

	SetPosition(0.0f, 0.0f, 0.0f);
}


ScreenTextureObject::~ScreenTextureObject()
{
}

void ScreenTextureObject::Update(float fTimeElapsed)
{
}
