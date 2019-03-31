#pragma once
#include "Object000_BaseObject.h"


class SkyBox :
	public BaseObject
{
public:
	SkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~SkyBox();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, BaseCamera *pCamera = nullptr);
};

