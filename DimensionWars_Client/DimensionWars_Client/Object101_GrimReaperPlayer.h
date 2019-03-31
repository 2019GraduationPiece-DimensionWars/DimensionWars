#pragma once
#include "Object100_BasePlayer.h"


class GrimReaperPlayer :
	public BasePlayer
{
public:
	GrimReaperPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = nullptr);
	virtual ~GrimReaperPlayer();

	virtual void OnPrepareRender();
	virtual BaseCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);
};

