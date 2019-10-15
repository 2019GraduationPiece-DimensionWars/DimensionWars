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

	if (m_pd3dTextureCoord0Buffer) m_pd3dTextureCoord0Buffer->Release();
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
	if (m_pd3dTangentBuffer) m_pd3dTangentBuffer->Release();
	if (m_pd3dBiTangentBuffer) m_pd3dBiTangentBuffer->Release();

	if (m_pxmf4Colors) delete[] m_pxmf4Colors;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;
	if (m_pxmf3Tangents) delete[] m_pxmf3Tangents;
	if (m_pxmf3BiTangents) delete[] m_pxmf3BiTangents;
	if (m_pxmf2TextureCoords0) delete[] m_pxmf2TextureCoords0;
	if (m_pxmf2TextureCoords1) delete[] m_pxmf2TextureCoords1;
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

	if (m_pd3dTextureCoord0UploadBuffer) m_pd3dTextureCoord0UploadBuffer->Release();
	m_pd3dTextureCoord0UploadBuffer = nullptr;

	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();
	m_pd3dNormalUploadBuffer = nullptr;

	if (m_pd3dTangentUploadBuffer) m_pd3dTangentUploadBuffer->Release();
	m_pd3dTangentUploadBuffer = nullptr;

	if (m_pd3dBiTangentUploadBuffer) m_pd3dBiTangentUploadBuffer->Release();
	m_pd3dBiTangentUploadBuffer = nullptr;
}

void HierarchyMesh::OnPreRender(ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[5] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView, m_d3dNormalBufferView, m_d3dTangentBufferView, m_d3dBiTangentBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 3, pVertexBufferViews);
	
	//pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);
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
	unsigned int nReads = 0;

	int nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

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
				else if (!strcmp(pstrToken, "<Colors>:")) {
					int nColorsPerVertex = 0;
					nReads = (unsigned int)::fread(&nColors, sizeof(int), 1, pInFile);
					nReads = (unsigned int)::fread(&nColorsPerVertex, sizeof(int), 1, pInFile);
					if (!strcmp(pstrToken, "<Color>:")) {
						int colorIdx = 0;
						nReads = (unsigned int)::fread(&colorIdx, sizeof(int), 1, pInFile);
						if (nColors > 0) {
							m_nType |= VERTEXT_COLOR;
							m_pxmf4Colors = new XMFLOAT4[nColors];
							nReads = (unsigned int)::fread(m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
						}
					}
				}
				else if (!strcmp(pstrToken, "<UVs>:")) {					
					int nUVsPerVertex = 0;
					nReads = (unsigned int)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
					nReads = (unsigned int)::fread(&nUVsPerVertex, sizeof(int), 1, pInFile);
					//ConsolePrint("nTextureCoords : %d, nUVsPerVertex : %d, 실 정점 수 : %d\n", nTextureCoords, nUVsPerVertex, m_nVertices);
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<UV>:")) {						
						int texIdx = 0;
						nReads = (unsigned int)::fread(&texIdx, sizeof(int), 1, pInFile);
						//ConsolePrint("texIdx : %d\n", texIdx);
						if (nTextureCoords > 0) {
							m_nType |= VERTEXT_TEXTURE_COORD0;
							m_pxmf2TextureCoords0 = new XMFLOAT2[nTextureCoords];
							nReads = (unsigned int)::fread(m_pxmf2TextureCoords0, sizeof(XMFLOAT2), nTextureCoords, pInFile);
							
							//for (int i = 0; i < nTextureCoords; ++i) ConsolePrint ("[%d]번째 텍스쳐 좌표 (%.3f, %.3f)\n", i, m_pxmf2TextureCoords0[i].x, m_pxmf2TextureCoords0[i].y);
							
							//float temp = m_pxmf2TextureCoords0->x;
							//m_pxmf2TextureCoords0->x = m_pxmf2TextureCoords0->y;
							//m_pxmf2TextureCoords0->y = temp;

						
							//m_pxmf2TextureCoords0->x = 1.0f - m_pxmf2TextureCoords0->x;
							//m_pxmf2TextureCoords0->y = 1.0f - m_pxmf2TextureCoords0->y;
							
							m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

							m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
							m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
							m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
						}
					}
				}
				else if (!strcmp(pstrToken, "<Normals>:")) {				
					int nNormalsPerVertex = 0;
					nReads = (unsigned int)::fread(&nNormals, sizeof(int), 1, pInFile);
					nReads = (unsigned int)::fread(&nNormalsPerVertex, sizeof(int), 1, pInFile);
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<Normal>:")) {
						int normIdx = 0;
						nReads = (unsigned int)::fread(&normIdx, sizeof(int), 1, pInFile);
						if (nNormals > 0) {
							m_nType |= VERTEXT_NORMAL;
							m_pxmf3Normals = new XMFLOAT3[nNormals];
							nReads = (unsigned int)::fread(m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);

							m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

							m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
							m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
							m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
						}
					}
				}
				else if (!strcmp(pstrToken, "<Tangents>:")) {
					int nTangentsPerVertex = 0;
					nReads = (unsigned int)::fread(&nTangents, sizeof(int), 1, pInFile);
					nReads = (unsigned int)::fread(&nTangentsPerVertex, sizeof(int), 1, pInFile);
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<Tangent>:")) {
						int tangentsIdx = 0;
						nReads = (unsigned int)::fread(&tangentsIdx, sizeof(int), 1, pInFile);
						if (nTangents > 0) {
							m_nType |= VERTEXT_TANGENT;
							m_pxmf3Tangents = new XMFLOAT3[nTangents];
							nReads = (unsigned int)::fread(m_pxmf3Tangents, sizeof(XMFLOAT3), nTangents, pInFile);

							m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Tangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);

							m_d3dTangentBufferView.BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
							m_d3dTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
							m_d3dTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
						}
					}
				}
				else if (!strcmp(pstrToken, "<BiTangents>:")) {
					int nBinormalsPerVertex = 0;
					nReads = (unsigned int)::fread(&nBiTangents, sizeof(int), 1, pInFile);
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<BiTangent>:")) {
						int biTangentsIdx = 0;
						nReads = (unsigned int)::fread(&biTangentsIdx, sizeof(int), 1, pInFile);
						if (nBiTangents > 0) {
							m_pxmf3BiTangents = new XMFLOAT3[nBiTangents];
							nReads = (unsigned int)::fread(m_pxmf3BiTangents, sizeof(XMFLOAT3), nBiTangents, pInFile);

							m_pd3dBiTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3BiTangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBiTangentUploadBuffer);

							m_d3dBiTangentBufferView.BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
							m_d3dBiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
							m_d3dBiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
						}
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
