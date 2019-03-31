#pragma once
#include "Mesh002_HierarchyMesh.h"

class SkinnedFrameObject;


class SkinnedMesh : public HierarchyMesh
{
public:
	SkinnedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	~SkinnedMesh();

protected:
	XMFLOAT4						*m_pxmf4Colors = nullptr;
	XMFLOAT3						*m_pxmf3Normals = nullptr;
	XMFLOAT3						*m_pxmf3Tangents = nullptr;
	XMFLOAT3						*m_pxmf3BiTangents = nullptr;
	XMFLOAT2						*m_pxmf2TextureCoords0 = nullptr;
	XMFLOAT2						*m_pxmf2TextureCoords1 = nullptr;

	ID3D12Resource					*m_pd3dTextureCoord0Buffer = nullptr;
	ID3D12Resource					*m_pd3dTextureCoord0UploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord0BufferView;

	ID3D12Resource					*m_pd3dTextureCoord1Buffer = nullptr;
	ID3D12Resource					*m_pd3dTextureCoord1UploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord1BufferView;

	ID3D12Resource					*m_pd3dNormalBuffer = nullptr;
	ID3D12Resource					*m_pd3dNormalUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dNormalBufferView;

	ID3D12Resource					*m_pd3dTangentBuffer = nullptr;
	ID3D12Resource					*m_pd3dTangentUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTangentBufferView;

	ID3D12Resource					*m_pd3dBiTangentBuffer = nullptr;
	ID3D12Resource					*m_pd3dBiTangentUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBiTangentBufferView;


	int								m_nBonesPerVertex = 4;

	XMINT4							*m_pxmn4BoneIndices = nullptr;

	ID3D12Resource					*m_pd3dBoneIndexBuffer = nullptr;
	ID3D12Resource					*m_pd3dBoneIndexUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneIndexBufferView;

	XMFLOAT4						*m_pxmf4BoneWeights = nullptr;

	ID3D12Resource					*m_pd3dBoneWeightBuffer = nullptr;
	ID3D12Resource					*m_pd3dBoneWeightUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneWeightBufferView;

public:
	int								m_nSkinningBones = 0;

	char							(*m_ppstrSkinningBoneNames)[64];
	SkinnedFrameObject				**m_ppSkinningBoneFrameCaches = NULL;

	XMFLOAT4X4						*m_pxmf4x4BindPoseBoneOffsets = NULL;

	ID3D12Resource					*m_pd3dcbBindPoseBoneOffsets = NULL;
	XMFLOAT4X4						*m_pcbxmf4x4MappedBindPoseBoneOffsets = NULL; //Transposed

	ID3D12Resource					*m_pd3dcbSkinningBoneTransforms = NULL;
	XMFLOAT4X4						*m_pcbxmf4x4MappedSkinningBoneTransforms = NULL;

public:
	void PrepareSkinning(SkinnedFrameObject *pModelRootObject);
	void LoadSkinDeformationsFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FILE *pInFile);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList *pd3dCommandList, void *pContext);
};

