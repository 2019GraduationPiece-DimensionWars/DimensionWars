#pragma once
#include "Object002_SkinnedFrameObject.h"
class SkinModelObject :
	public SkinnedFrameObject
{
public:
	SkinModelObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LoadedModelInfo *pModel, char* pFilePathName, int nAnimationTracks);
	virtual ~SkinModelObject();
};

