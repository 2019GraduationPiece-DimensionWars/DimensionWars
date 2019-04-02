#include "stdafx.h"
#include "Object006_TextureRectObject.h"
#include "Mesh005_TextureRectangleMesh.h"
#include "Shader005_TextureRectangleShader.h"
#include "Scene000_BaseScene.h"
#include "Texture.h"
#include "Material.h"

TextureRectObject::TextureRectObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, wchar_t* pTextureFileName, unsigned int nMaterials) : BaseObject(nMaterials)
{
	TextureRectangleMesh *pTitleMesh = new TextureRectangleMesh(pd3dDevice, pd3dCommandList, 200.0f, 200.0f, 0.0f, 0.0f, 0.0f, 50.0f);
	SetMesh(pTitleMesh);
	
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Texture * titleImage = new Texture(1, RESOURCE_TEXTURE2D, 0);
	titleImage->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pTextureFileName, 0);
	
	TextureRectangleShader *pTextureShader = new TextureRectangleShader();
	pTextureShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTextureShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	BaseScene::CreateShaderResourceViews(pd3dDevice, titleImage, 14, false);

	Material * titleMaterial = new Material(1);
	titleMaterial->SetTexture(titleImage);
	titleMaterial->SetShader(pTextureShader);

	SetMaterial(0, titleMaterial);

	SetPosition(0.0f, 0.0f, 0.0f);
}


TextureRectObject::~TextureRectObject()
{
}

void TextureRectObject::Update(float fTimeElapsed)
{
}
