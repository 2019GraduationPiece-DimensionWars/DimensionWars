#include "stdafx.h"
#include "Mesh002_HierarchyMesh.h"


HierarchyMesh::HierarchyMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : BaseMesh(pd3dDevice, pd3dCommandList)
{
}

HierarchyMesh::~HierarchyMesh()
{
	if (m_pd3dPositionBuffer)
		m_pd3dPositionBuffer->Release();

	if (m_nSubMeshes > 0) {
		for (int i = 0; i < m_nSubMeshes; i++) {
			if (m_ppd3dSubSetIndexBuffers[i])
				m_ppd3dSubSetIndexBuffers[i]->Release();
			if (m_ppnSubSetIndices[i])
				delete[] m_ppnSubSetIndices[i];
		}
		if (m_ppd3dSubSetIndexBuffers)
			delete[] m_ppd3dSubSetIndexBuffers;
		if (m_pd3dSubSetIndexBufferViews)
			delete[] m_pd3dSubSetIndexBufferViews;

		if (m_pnSubSetIndices)
			delete[] m_pnSubSetIndices;
		if (m_ppnSubSetIndices)
			delete[] m_ppnSubSetIndices;
	}

	if (m_pxmf3Positions)
		delete[] m_pxmf3Positions;
}

void HierarchyMesh::ReleaseUploadBuffers()
{
	if (m_pd3dPositionUploadBuffer) 
		m_pd3dPositionUploadBuffer->Release();
	m_pd3dPositionUploadBuffer = nullptr;

	if ((m_nSubMeshes > 0) && m_ppd3dSubSetIndexUploadBuffers) {
		for (int i = 0; i < m_nSubMeshes; i++)
			if (m_ppd3dSubSetIndexUploadBuffers[i]) 
				m_ppd3dSubSetIndexUploadBuffers[i]->Release();
		if (m_ppd3dSubSetIndexUploadBuffers) 
			delete[] m_ppd3dSubSetIndexUploadBuffers;
		m_ppd3dSubSetIndexUploadBuffers = nullptr;
	}
}

void HierarchyMesh::OnPreRender(ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);
}

void HierarchyMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, int nSubSet)
{
	UpdateShaderVariables(pd3dCommandList);

	OnPreRender(pd3dCommandList, NULL);

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes)) {
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

void HierarchyMesh::OnPostRender(ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
}


void HierarchyMesh::LoadMeshFromFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, FILE * pInFile)
{
	char pstrToken[64] = { '\0' };
	unsigned char nStrLength = 0;
	unsigned int nReads = 0;

	int nColors = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	::ReadStringFromFile(pInFile, m_pstrMeshName);

	while (true) {
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Bounds>:")) {
			nReads = (unsigned int)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (unsigned int)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<ControlPoints>:")) {
			nReads = (unsigned int)::fread(&m_nVertices, sizeof(int), 1, pInFile);
			if (m_nVertices > 0) {
				m_nType |= VERTEXT_POSITION;
				m_pxmf3Positions = new XMFLOAT3[m_nVertices];
				nReads = (unsigned int)::fread(m_pxmf3Positions, sizeof(XMFLOAT3), m_nVertices, pInFile);

				m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

				m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
				m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Polygons>:")) {
			int nPolygons = 0;
			nReads = (unsigned int)::fread(&nPolygons, sizeof(int), 1, pInFile);
			while (true) {
				::ReadStringFromFile(pInFile, pstrToken);

				if (!strcmp(pstrToken, "<Indices>:")) {
					m_nSubMeshes = 1;
					m_pnSubSetIndices = new int[m_nSubMeshes];
					m_ppnSubSetIndices = new unsigned int*[m_nSubMeshes];

					m_ppd3dSubSetIndexBuffers = new ID3D12Resource*[m_nSubMeshes];
					m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource*[m_nSubMeshes];
					m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

					nReads = (unsigned int)::fread(&(m_pnSubSetIndices[0]), sizeof(int), 1, pInFile);
					if (m_pnSubSetIndices[0] > 0) {
						m_ppnSubSetIndices[0] = new unsigned int[m_pnSubSetIndices[0]];
						nReads = (unsigned int)::fread(m_ppnSubSetIndices[0], sizeof(unsigned int), m_pnSubSetIndices[0], pInFile);

						m_ppd3dSubSetIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0], sizeof(unsigned int) * m_pnSubSetIndices[0], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[0]);

						m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
						m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
						m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(unsigned int) * m_pnSubSetIndices[0];
					}
				}
				else if (!strcmp(pstrToken, "</Polygons>"))
					break;
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
			break;
	}
}
