#pragma once

class Vertex
{
public:
	XMFLOAT3						m_xmf3Position;

public:
	Vertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	Vertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~Vertex() { }
};


class BaseMesh
{
public:
	BaseMesh();
	BaseMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);

private:
	int	m_nReferences = 0;

public:
	void AddRef() { ++m_nReferences; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void ReleaseShaderVariables() { }

	virtual void ReleaseUploadBuffers() { }

protected:
	XMFLOAT3						m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3						m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT3						*m_pxmf3Positions = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	unsigned int					m_nSlot = 0;
	unsigned int					m_nStride = 0;
	unsigned int					m_nVertices = 0;
	unsigned int					m_nOffset = 0;

	unsigned int					m_nType = 0;

	ID3D12Resource					*m_pd3dPositionBuffer = nullptr;
	ID3D12Resource					*m_pd3dPositionUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

public:
	unsigned int GetType() const { return m_nType; }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet = 0) = 0;
};


