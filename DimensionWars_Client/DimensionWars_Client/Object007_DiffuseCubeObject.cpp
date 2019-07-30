#include "stdafx.h"
#include "Material.h"
#include "Object007_DiffuseCubeObject.h"
#include "Shader006_DiffuseShader.h"
#include "Mesh006_DiffuseCubeMesh.h"
#include "Mesh004_SkyBoxMesh.h"
#include "Texture.h"
#include "Shader001_SkyBoxShader.h"
#include "Scene000_BaseScene.h"
#include "Shader005_TextureRectangleShader.h"
#include "Mesh005_TextureRectangleMesh.h"
#include "Shader008_CubeShader.h"

DiffuseCubeObject::DiffuseCubeObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float size) : BaseObject(1)
{
	DiffuseCubeMesh *pDiffuseCubeMesh = new DiffuseCubeMesh(pd3dDevice, pd3dCommandList, size, size, size);
	SetMesh(pDiffuseCubeMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	DiffuseShader *pShader = new DiffuseShader();
	pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Material *pCubeMaterial = new Material(0);
	pCubeMaterial->SetShader(pShader);

	SetMaterial(0, pCubeMaterial);
}


DiffuseCubeObject::~DiffuseCubeObject()
{
}

void DiffuseCubeObject::Build(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float size)
{
	DiffuseCubeMesh *pDiffuseCubeMesh = new DiffuseCubeMesh(pd3dDevice, pd3dCommandList, size, size, size);
	SetMesh(pDiffuseCubeMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	DiffuseShader *pShader = new DiffuseShader();
	pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Material *pCubeMaterial = new Material(0);
	pCubeMaterial->SetShader(pShader);

	SetMaterial(0, pCubeMaterial);
}


TextureCubeObject::TextureCubeObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float size) : BaseObject(1)
{
	DiffuseCubeMesh *pSkyBoxMesh = new DiffuseCubeMesh(pd3dDevice, pd3dCommandList, size, size, size);
	SetMesh(pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Texture *pSkyBoxTexture = new Texture(1, RESOURCE_TEXTURE_CUBE, 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/SampleCube7.dds", 0);

	CubeShader *pSkyBoxShader = new CubeShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	BaseScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 16, false);

	Material *pSkyBoxMaterial = new Material(1);
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(0, pSkyBoxMaterial);

}
TextureCubeObject::~TextureCubeObject()
{
}

void TextureCubeObject::Build(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float size)
{
	/*SkyBoxMesh *pSkyBoxMesh = new SkyBoxMesh(pd3dDevice, pd3dCommandList, size, size, size);
	SetMesh(pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Texture *pSkyBoxTexture = new Texture(1, RESOURCE_TEXTURE_CUBE, 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/TitleTest800x600.dds", 0);

	TextureRectangleShader *pSkyBoxShader = new TextureRectangleShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	BaseScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 15, false);

	Material *pSkyBoxMaterial = new Material(1);
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(0, pSkyBoxMaterial);*/



}