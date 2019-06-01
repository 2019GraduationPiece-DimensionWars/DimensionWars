#pragma once
#include "Object100_BasePlayer.h"

class AnimationController;
class GrimReaperPlayer :
	public BasePlayer
{
private:
	bool SecondAttackTrigger = false;
	bool ThirdAttackTrigger = false;
public:
	enum State {
		Full = 0,
		Idle = 1,
		OnHit = 2,
		Guard = 3,
		Burf = 4,
		Hide_Invasion = 5,
		// Full_Attack = 6,
		First_Attack = 6,
		Second_Attack = 7,
		Third_Attack = 8,
		Slash_Wave = 9,
		Beheading = 10,
		Move_Forward = 11,
		Move_Right = 13,	//Move_Right = 12,
		Move_Left = 12,		//Move_Left = 13,
		Move_Backward = 14,
		Down = 15
	};
	State state = Idle;

	GrimReaperPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, RuntimeFrameWork * pFramework);
	virtual ~GrimReaperPlayer();

	virtual void OnPrepareRender();
	virtual BaseCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);

	virtual void ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed) override;

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	virtual bool isCancleEnabled() override;
	virtual void SendSlash();

	
};

