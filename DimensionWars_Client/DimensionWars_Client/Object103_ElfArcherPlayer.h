#pragma once
#include "Object100_BasePlayer.h"

class AnimationController;

class ElfArcherPlayer :
	public BasePlayer
{
private:
	bool SecondShotTrigger = false;
	bool ThirdShotTrigger = false;
public:
	enum State {
		Full = 0,
		Idle = 1,
		OnHit = 2,
		Guard = 3,
		Burf = 4,
		First_Shot = 5,
		Second_Shot = 6,
		Third_Shot = 7,
		Guide_Shot = 8,
		Snipe = 9,
		Jump = 10,
		Jump_Attack = 11,
		Fall = 12,
		Fall_Attack = 13,
		Move_Forward = 14,
		Move_Right_Forward = 16,	// Move_Right_Forward = 15,
		Move_Left_Forward = 15,		// Move_Left_Forward = 16,
		Move_Right = 18,			// Move_Right = 17,
		Move_Left = 17,				// Move_Left = 18,
		Move_Backward = 19,
		Move_Right_Backward = 21,	// Move_Right_Backward = 20,
		Move_Left_Backward = 20,	// Move_Left_Backward = 21,
		Down = 22
	};
	State state = Idle;

	ElfArcherPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, RuntimeFrameWork * pFramework);
	virtual ~ElfArcherPlayer();


	virtual void OnPrepareRender();
	virtual BaseCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);

	virtual void ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed) override;

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	virtual bool isCancleEnabled() override;
};
