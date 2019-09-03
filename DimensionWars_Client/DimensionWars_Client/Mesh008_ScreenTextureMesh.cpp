#include "stdafx.h"
#include "Mesh008_ScreenTextureMesh.h"
#include "Mesh005_TextureRectangleMesh.h"


ScreenTextureMesh::ScreenTextureMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, float left, float top, float right, float bottom)
{
	m_nVertices = 6;
	m_nStride = sizeof(TexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	TexturedVertex pVertices[6];

	pVertices[0] = TexturedVertex(XMFLOAT3(right, top, 0.0f), XMFLOAT2(1.0f, 0.0f));
	pVertices[1] = TexturedVertex(XMFLOAT3(right, bottom, 0.0f), XMFLOAT2(1.0f, 1.0f));
	pVertices[2] = TexturedVertex(XMFLOAT3(left, bottom, 0.0f), XMFLOAT2(0.0f, 1.0f));
	pVertices[3] = TexturedVertex(XMFLOAT3(left, bottom, 0.0f), XMFLOAT2(0.0f, 1.0f));
	pVertices[4] = TexturedVertex(XMFLOAT3(left, top, 0.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[5] = TexturedVertex(XMFLOAT3(right, top, 0.0f), XMFLOAT2(1.0f, 0.0f));
	
	//(0.0f, 0.5f, 0.0f),
	//	(0.5f, -0.5f, 0.0f),
	//	(-0.5f, -0.5f, 0.0f)
	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = m_nStride;
	m_d3dPositionBufferView.SizeInBytes = m_nStride * m_nVertices;
}

ScreenTextureMesh::~ScreenTextureMesh()
{
}

void ScreenTextureMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, int subset)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);

	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}
