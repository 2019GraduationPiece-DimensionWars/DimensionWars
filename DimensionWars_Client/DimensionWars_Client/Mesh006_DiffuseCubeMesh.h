#pragma once
#include "Mesh001_NoneHierarchyMesh.h"


class DiffusedVertex : public Vertex 
{
protected:
	//정점의 색상이다.   
	XMFLOAT4 m_xmf4Diffuse;

public:
	DiffusedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); }
	DiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse; }
	DiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = xmf3Position; m_xmf4Diffuse = xmf4Diffuse; }
	~DiffusedVertex() { }
};

class DiffuseCubeMesh :
	public NoneHierarchyMesh
{
public:
	DiffuseCubeMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	~DiffuseCubeMesh();
};

