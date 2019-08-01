#pragma once

#include "Object002_SkinnedFrameObject.h"

class RuntimeFrameWork;

class PortalObject :
	public SkinnedFrameObject
{
protected:
	RuntimeFrameWork*			m_pFramework = nullptr;
public:
	PortalObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, RuntimeFrameWork * pFramework);
	~PortalObject();


	void SetFramework(RuntimeFrameWork* p) { m_pFramework = p; }
};

