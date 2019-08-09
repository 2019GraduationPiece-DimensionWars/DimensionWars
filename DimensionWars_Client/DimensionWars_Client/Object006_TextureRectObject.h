#pragma once
#include "Object000_BaseObject.h"
class Texture;
class TextureRectObject :
	public BaseObject
{
public:
	TextureRectObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float width = 200.0f, float height = 200.0f, float depth = 0.0f, unsigned int nMaterials = 1);
	virtual ~TextureRectObject();

	virtual void Update(float fTimeElapsed);
};

