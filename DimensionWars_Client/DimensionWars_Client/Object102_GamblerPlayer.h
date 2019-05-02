#pragma once
#include "Object100_BasePlayer.h"
class GamblerPlayer :
	public BasePlayer
{
public:
	enum State {
		Full = 0,
		Idle = 1,
		OnHit = 2,
		Guard = 3,
		Burf = 4,
		Shffle = 5,
		Idle_Attack = 6,
		Multi_Shot = 7,
		Wild_Card = 8,
		Jump = 9,
		Jump_Attack = 10,
		Fall = 11,
		Fall_Attack = 12,
		Move_Forward = 13,
		Move_Forward_Attack = 14,
		Move_Right_Forward = 15,
		Move_Right_Forward_Attack = 16,
		Move_Left_Forward = 17,
		Move_Left_Forward_Attack = 18,
		Move_Right = 19,
		Move_Right_Attack = 20,
		Move_Left = 21,
		Move_Left_Attack = 22,
		Move_Backward = 23,
		Move_Backward_Attack = 24,
		Move_Right_Backward = 25,
		Move_Right_Backward_Attack = 26,
		Move_Left_Backward = 27,
		Move_Left_Backward_Attack = 28,
		Down = 29
	};
	State state;

	GamblerPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext);
	virtual ~GamblerPlayer();


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
		case Move_Left_Forward:
		case Move_Right_Forward:		
		case Move_Left:
		case Move_Right:
		case Move_Backward:
		case Move_Left_Backward:
		case Move_Right_Backward:
			return true;
		default:	// 위 상태가 아니면 현재 모션을 캔슬 할 수 없다.
			return false;
		}
	}
};
