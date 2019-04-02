#pragma once
#include "Object000_BaseObject.h"
class TextureRectObject :
	public BaseObject
{
public:
	TextureRectObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, wchar_t* pTextureFileName, unsigned int nMaterials = 1);
	virtual ~TextureRectObject();

	virtual void Update(float fTimeElapsed);
};

