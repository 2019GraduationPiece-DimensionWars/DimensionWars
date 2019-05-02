#pragma once
#include "Object100_BasePlayer.h"


class GrimReaperPlayer :
	public BasePlayer
{
public:
	enum State {
		Full = 0,
		Idle = 1,
		OnHit = 2,
		Guard = 3,
		Burf = 4,
		Hide_Invasion = 5,
		Full_Attack = 6,
		Slash_Wave = 7,
		Beheading = 8,
		Move_Forward = 9,
		Move_Right = 10,
		Move_Left = 11,
		Move_Backward = 12,
		Down = 13
	};
	State state;

	GrimReaperPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = nullptr);
	virtual ~GrimReaperPlayer();

	virtual void OnPrepareRender();
	virtual BaseCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);

	virtual void ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed) override;

	virtual bool isCancleEnabled() const {
		switch (state) {
		case Idle:
		case Move_Forward:
		case Move_Left:
		case Move_Right:
		case Move_Backward:
			return true;
		default:	// 위 상태가 아니면 현재 모션을 캔슬 할 수 없다.
			return false;
		}
	}
};

