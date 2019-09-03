#pragma once
#include "Object000_BaseObject.h"

class Texture;

class ScreenTextureObject : public BaseObject
{
public:
	ScreenTextureObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float left = 0.0f, float top = 1.0f, float right = 1.0f, float bottom = 0.0f, unsigned int nMaterials = 1);
	virtual ~ScreenTextureObject();

	virtual void Update(float fTimeElapsed);
};

