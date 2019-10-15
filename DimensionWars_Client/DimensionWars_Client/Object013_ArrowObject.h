#pragma once
#include "Object009_ProjectileObject.h"

class ArrowObject :
	public ProjectileObject
{
public:
	ArrowObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, RuntimeFrameWork * pFramework, unsigned int index = 0);
	~ArrowObject();
};

