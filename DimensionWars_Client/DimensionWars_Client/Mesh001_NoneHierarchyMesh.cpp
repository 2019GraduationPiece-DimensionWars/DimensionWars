#include "stdafx.h"
#include "Mesh001_NoneHierarchyMesh.h"


NoneHierarchyMesh::NoneHierarchyMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList) : BaseMesh(pd3dDevice, pd3dCommandList)
{
}

NoneHierarchyMesh::NoneHierarchyMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, UINT nVertices, XMFLOAT3 * pxmf3Positions, UINT nIndices, UINT * pnIndices)
{
}

NoneHierarchyMesh::~NoneHierarchyMesh()
{
	if (m_pd3dVertexBuffer)
		m_pd3dVertexBuffer->Release();
	if (m_pd3dIndexBuffer)
		m_pd3dIndexBuffer->Release();
}

void NoneHierarchyMesh::ReleaseUploadBuffers()
{
	if (m_pd3dVertexUploadBuffer) 
		m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = nullptr;
	if (m_pd3dIndexUploadBuffer)
		m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = nullptr;
}

void NoneHierarchyMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer) {
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}
