#pragma once
#include "Mesh000_BaseMesh.h"


class HierarchyMesh : public BaseMesh
{
public:
	HierarchyMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) ;
	virtual ~HierarchyMesh();

public:
	char							m_pstrMeshName[64] = { 0 };

protected:
	ID3D12Resource					*m_pd3dPositionBuffer = nullptr;
	ID3D12Resource					*m_pd3dPositionUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	int								m_nSubMeshes = 0;
	int								*m_pnSubSetIndices = nullptr;
	unsigned int					**m_ppnSubSetIndices = nullptr;

	ID3D12Resource					**m_ppd3dSubSetIndexBuffers = nullptr;
	ID3D12Resource					**m_ppd3dSubSetIndexUploadBuffers = nullptr;
	D3D12_INDEX_BUFFER_VIEW			*m_pd3dSubSetIndexBufferViews = nullptr;

public:
	UINT GetType() { return(m_nType); }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void ReleaseShaderVariables() { }

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList *pd3dCommandList, void *pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet);
	virtual void OnPostRender(ID3D12GraphicsCommandList *pd3dCommandList, void *pContext);

	virtual void LoadMeshFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FILE *pInFile);
};
