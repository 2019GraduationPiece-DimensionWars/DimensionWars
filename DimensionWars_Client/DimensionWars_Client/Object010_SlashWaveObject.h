#pragma once
#include "Object009_ProjectileObject.h"

class SlashWaveObject :
	public ProjectileObject
{
public:
	SlashWaveObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, RuntimeFrameWork * pFramework);
	virtual ~SlashWaveObject();
};

