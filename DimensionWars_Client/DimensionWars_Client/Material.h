#pragma once

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40


class BaseObject;
class SkinnedFrameObject;
class BaseShader;
class Texture;


class Material
{
private:
	int	m_nReferences = 0;

public:
	unsigned int					m_nType = 0x00;

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

	Material() = delete;
	Material(unsigned int nTextures = 1);
	virtual ~Material();
	void AddRef() { ++m_nReferences; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	BaseShader						*m_pShader = nullptr;

	XMFLOAT4						m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4						m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	void SetShader(BaseShader *pShader);
	void SetMaterialType(unsigned int nType) { m_nType |= nType; }
	void SetTexture(Texture *pTexture, unsigned int nTexture = 0);

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void ReleaseUploadBuffers();

	int 							m_nTextures = 0;
	_TCHAR							(*m_ppstrTextureNames)[64] = nullptr;
	Texture							**m_ppTextures = nullptr; //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR *pwstrTextureName, Texture **ppTexture, SkinnedFrameObject *pParent, FILE *pInFile, BaseShader *pShader);

	static BaseShader					*m_pWireFrameShader;
	static BaseShader					*m_pSkinnedAnimationWireFrameShader;

	static BaseShader					*m_pStandardShader;
	static BaseShader					*m_pSkinnedAnimationShader;


	static void Material::PrepareShaders(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	void SetWireFrameShader() { Material::SetShader(m_pWireFrameShader); }
	void SetSkinnedAnimationWireFrameShader() { Material::SetShader(m_pSkinnedAnimationWireFrameShader); }
	void SetStandardShader() { Material::SetShader(m_pStandardShader); }
	void SetSkinnedAnimationShader() { Material::SetShader(m_pSkinnedAnimationShader); }
};

