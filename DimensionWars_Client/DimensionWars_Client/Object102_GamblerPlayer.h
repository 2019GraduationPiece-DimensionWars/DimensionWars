#pragma once
#include "Object100_BasePlayer.h"
class AnimationController;

class GamblerPlayer :
	public BasePlayer
{
private:
	bool isShot1 = false;
public:
	enum State {
		Full = 0,
		Idle = 1,
		OnHit = 2,
		Guard = 3,
		Burf = 4,
		Shuffle = 5,
		Idle_Attack = 6,
		Multi_Shot = 7,
		Wild_Card = 8,
		Jump = 9,
		Jump_Attack = 10,
		Fall = 11,
		Fall_Attack = 12,
		Move_Forward = 13,
		Move_Forward_Attack1 = 14,
		Move_Forward_Attack2 = 15,
		Move_Right_Forward = 19	,			// Move_Right_Forward = 16,
		Move_Right_Forward_Attack1 = 20,	// Move_Right_Forward_Attack1 = 17,
		Move_Right_Forward_Attack2 = 21,	// Move_Right_Forward_Attack2 = 18,
		Move_Left_Forward = 16,				// Move_Left_Forward = 19,
		Move_Left_Forward_Attack1 = 17,		// Move_Left_Forward_Attack1 = 20,
		Move_Left_Forward_Attack2 = 18,		// Move_Left_Forward_Attack2 = 21,
		Move_Right = 25,					// Move_Right = 22,
		Move_Right_Attack1 = 26,			// Move_Right_Attack1 = 23,
		Move_Right_Attack2 = 27,			// Move_Right_Attack2 = 24,
		Move_Left = 22,						// Move_Left = 25,
		Move_Left_Attack1 = 23,				// Move_Left_Attack1 = 26,
		Move_Left_Attack2 = 24,				// Move_Left_Attack2 = 27,
		Move_Backward = 28,
		Move_Backward_Attack1 = 29,
		Move_Backward_Attack2 = 30,
		Move_Right_Backward = 34,			//Move_Right_Backward = 31,
		Move_Right_Backward_Attack1 = 35,	//Move_Right_Backward_Attack1 = 32,
		Move_Right_Backward_Attack2 = 36,	//Move_Right_Backward_Attack2 = 33,
		Move_Left_Backward = 31,			//Move_Left_Backward = 34,
		Move_Left_Backward_Attack1 = 32,	//Move_Left_Backward_Attack1 = 35,
		Move_Left_Backward_Attack2 = 33,	//Move_Left_Backward_Attack2 = 36,
		Down = 37
	};
	State state = Idle;

	GamblerPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, RuntimeFrameWork * pFramework, unsigned int index = 0);
	virtual ~GamblerPlayer();


	virtual void OnPrepareRender();
	virtual BaseCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);

	virtual void ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed) override;

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	virtual bool isCancleEnabled() override;

	virtual void SendCard();
	virtual void SendjumpDirection();
};
