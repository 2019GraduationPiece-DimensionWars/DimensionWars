#include "stdafx.h"
#include "Object008_HeightmapTerrain.h"
#include "HeightMapImage.h"
#include "Mesh007_HeightMapGridMesh.h"
#include "Shader007_TerrainShader.h"
#include "Texture.h"
#include "Material.h"
#include "Scene000_BaseScene.h"




HeightMapTerrain::HeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : BaseObject(1)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new HeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	HeightMapGridMesh *pMesh = new HeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nWidth, nLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
	SetMesh(pMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Texture *pTerrainBaseTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainBaseTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Detail_Texture_1.dds", 0);

	Texture *pTerrainDetailTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainDetailTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Detail_Texture_2.dds", 0);

	TerrainShader *pTerrainShader = new TerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	BaseScene::CreateShaderResourceViews(pd3dDevice, pTerrainBaseTexture, 13, false);
	BaseScene::CreateShaderResourceViews(pd3dDevice, pTerrainDetailTexture, 14, false);

	Material *pTerrainMaterial = new Material(2);
	pTerrainMaterial->SetTexture(pTerrainBaseTexture, 0);
	pTerrainMaterial->SetTexture(pTerrainDetailTexture, 1);
	pTerrainMaterial->SetShader(pTerrainShader);

	SetMaterial(0, pTerrainMaterial);
}


HeightMapTerrain::~HeightMapTerrain()
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

float HeightMapTerrain::GetHeight(float x, float z, bool bReverseQuad)
{
	return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y);
}

XMFLOAT3 HeightMapTerrain::GetNormal(float x, float z)
{
	return (m_pHeightMapImage->GetPointToHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
}

int HeightMapTerrain::GetHeightMapWidth()
{
	return(m_pHeightMapImage->GetHeightMapWidth());
}

int HeightMapTerrain::GetHeightMapLength()
{
	return(m_pHeightMapImage->GetHeightMapLength());
}
