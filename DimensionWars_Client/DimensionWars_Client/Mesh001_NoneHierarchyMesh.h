#pragma once
#include "Mesh000_BaseMesh.h"

class NoneHierarchyMesh : public BaseMesh
{
public:
	NoneHierarchyMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	NoneHierarchyMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, UINT nIndices, UINT *pnIndices);
	virtual ~NoneHierarchyMesh();

	virtual void ReleaseUploadBuffers();

protected:
	ID3D12Resource					*m_pd3dVertexBuffer = nullptr;
	ID3D12Resource					*m_pd3dVertexUploadBuffer = nullptr;

	ID3D12Resource					*m_pd3dIndexBuffer = nullptr;
	ID3D12Resource					*m_pd3dIndexUploadBuffer = nullptr;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

	UINT							m_nStride = 0;

	UINT							m_nIndices = 0;
	UINT							m_nStartIndex = 0;
	int								m_nBaseVertex = 0;

public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet = 0);
};

