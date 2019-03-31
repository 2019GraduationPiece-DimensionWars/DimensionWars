#pragma once
#include "Mesh001_NoneHierarchyMesh.h"
class SkyBoxMesh :
	public NoneHierarchyMesh
{
public:
	SkyBoxMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f);
	virtual ~SkyBoxMesh();
};

