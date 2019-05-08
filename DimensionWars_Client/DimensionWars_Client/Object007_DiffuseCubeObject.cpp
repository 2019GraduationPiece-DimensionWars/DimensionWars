#include "stdafx.h"
#include "Material.h"
#include "Object007_DiffuseCubeObject.h"
#include "Shader006_DiffuseShader.h"
#include "Mesh006_DiffuseCubeMesh.h"


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
