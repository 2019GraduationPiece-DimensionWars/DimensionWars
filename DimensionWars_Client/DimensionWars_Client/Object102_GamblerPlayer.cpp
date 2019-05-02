#include "stdafx.h"
#include "Object102_GamblerPlayer.h"
#include "Camera002_ThirdPersonCamera.h"
#include "AnimationController.h"


GamblerPlayer::GamblerPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	LoadedModelInfo *GrimReaperModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Gambler.bin", nullptr);
	SetChild(GrimReaperModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new AnimationController(pd3dDevice, pd3dCommandList, 1, GrimReaperModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pSkinnedAnimationController->AddAnimationSet(0, 40.0 * keyFrameUnit, "Idle");
	m_pSkinnedAnimationController->AddAnimationSet(42.0 * keyFrameUnit, 62.0 * keyFrameUnit, "OnHit", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(64.0 * keyFrameUnit, 104.0 * keyFrameUnit, "Guard");
	m_pSkinnedAnimationController->AddAnimationSet(106.0 * keyFrameUnit, 131.0 * keyFrameUnit, "Burf", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(133.0 * keyFrameUnit, 173.0 * keyFrameUnit, "Shuffle", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(175.0 * keyFrameUnit, 195.0 * keyFrameUnit, "Idle Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(197.0 * keyFrameUnit, 217.0 * keyFrameUnit, "Multi Shot", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(219.0 * keyFrameUnit, 249.0 * keyFrameUnit, "Wild Card", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(252.0 * keyFrameUnit, 286.0 * keyFrameUnit, "Jump", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(288.0 * keyFrameUnit, 323.0 * keyFrameUnit, "Jump Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(325.0 * keyFrameUnit, 345.0 * keyFrameUnit, "Fall");
	m_pSkinnedAnimationController->AddAnimationSet(347.0 * keyFrameUnit, 367.0 * keyFrameUnit, "Fall Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(369.0 * keyFrameUnit, 399.0 * keyFrameUnit, "Move Forward");
	m_pSkinnedAnimationController->AddAnimationSet(401.0 * keyFrameUnit, 431.0 * keyFrameUnit, "Move Forward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(433.0 * keyFrameUnit, 463.0 * keyFrameUnit, "Move Right Forward");
	m_pSkinnedAnimationController->AddAnimationSet(465.0 * keyFrameUnit, 495.0 * keyFrameUnit, "Move Right Forward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(497.0 * keyFrameUnit, 527.0 * keyFrameUnit, "Move Left Forward");
	m_pSkinnedAnimationController->AddAnimationSet(529.0 * keyFrameUnit, 559.0 * keyFrameUnit, "Move Right Forward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(561.0 * keyFrameUnit, 591.0 * keyFrameUnit, "Move Right");
	m_pSkinnedAnimationController->AddAnimationSet(593.0 * keyFrameUnit, 623.0 * keyFrameUnit, "Move Right Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(625.0 * keyFrameUnit, 655.0 * keyFrameUnit, "Move Left");
	m_pSkinnedAnimationController->AddAnimationSet(657.0 * keyFrameUnit, 687.0 * keyFrameUnit, "Move Left Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(689.0 * keyFrameUnit, 719.0 * keyFrameUnit, "Move Backward");
	m_pSkinnedAnimationController->AddAnimationSet(689.0 * keyFrameUnit, 719.0 * keyFrameUnit, "Move Backward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(753.0 * keyFrameUnit, 783.0 * keyFrameUnit, "Move Right Backward");
	m_pSkinnedAnimationController->AddAnimationSet(785.0 * keyFrameUnit, 815.0 * keyFrameUnit, "Move Right Backward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(817.0 * keyFrameUnit, 847.0 * keyFrameUnit, "Move Left Backward");
	m_pSkinnedAnimationController->AddAnimationSet(849.0 * keyFrameUnit, 879.0 * keyFrameUnit, "Move Left Backward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(881.0 * keyFrameUnit, 904.0 * keyFrameUnit, "Down", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->SetAnimationSet(Idle);
	/*
	m_pSkinnedAnimationController->SetCallbackKeys(0, 0);
	#ifdef _WITH_SOUND_RESOURCE
	m_pSkinnedAnimationController->SetCallbackKey(1, 0, 0.1f, _T("Footstep01"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 1, 0.5f, _T("Footstep02"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 2, 0.9f, _T("Footstep03"));
	#else
	m_pSkinnedAnimationController->SetCallbackKey(1, 0, 0.1f, _T("Sound/Footstep01.wav"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 1, 0.5f, _T("Sound/Footstep02.wav"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 2, 0.9f, _T("Sound/Footstep03.wav"));
	#endif
	CAnimationCallbackHandler *pAnimationCallbackHandler = new CSoundCallbackHandler();
	m_pSkinnedAnimationController->SetAnimationCallbackHandler(1, pAnimationCallbackHandler);
	*/
	SetPlayerUpdatedContext(pContext);
	SetCameraUpdatedContext(pContext);

	if (GrimReaperModel) delete GrimReaperModel;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));

	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
}


GamblerPlayer::~GamblerPlayer()
{
}

void GamblerPlayer::OnPrepareRender()
{
	BasePlayer::OnPrepareRender();

	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationX(-90.0f), m_xmf4x4ToParent);
}

BaseCamera * GamblerPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(250.0f);
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 50.0f, -350.0f));
		m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);

	return(m_pCamera);
}

void GamblerPlayer::Update(float fTimeElapsed)
{
	BasePlayer::Update(fTimeElapsed);
}

void GamblerPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
}

void GamblerPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();

	ThirdPersonCamera *p3rdPersonCamera = (ThirdPersonCamera *)m_pCamera;
	p3rdPersonCamera->SetLookAt(GetPosition());
}

void GamblerPlayer::ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed)
{
	DWORD dwDirection = 0;
	if (isCancleEnabled()) {
		if (pKeysBuffer['w'] & 0xF0 || pKeysBuffer['W'] & 0xF0) {
			dwDirection |= DIR_FORWARD;
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Forward);
		}
		if (pKeysBuffer['s'] & 0xF0 || pKeysBuffer['S'] & 0xF0) {
			dwDirection |= DIR_BACKWARD;
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Backward);
		}
		if (pKeysBuffer['a'] & 0xF0 || pKeysBuffer['A'] & 0xF0) {
			dwDirection |= DIR_LEFT;
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Left);
		}
		if (pKeysBuffer['d'] & 0xF0 || pKeysBuffer['D'] & 0xF0) {
			dwDirection |= DIR_RIGHT;
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Right);
		}
		if ((dwDirection & DIR_FORWARD) && (dwDirection & DIR_LEFT))
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Left_Forward);
		if ((dwDirection & DIR_FORWARD) && (dwDirection & DIR_RIGHT))
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Right_Forward);
		if ((dwDirection & DIR_BACKWARD) && (dwDirection & DIR_LEFT))
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Left_Backward);
		if ((dwDirection & DIR_BACKWARD) && (dwDirection & DIR_RIGHT))
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Right_Backward);

		if ((dwDirection & DIR_LEFT) && (dwDirection & DIR_RIGHT)) {
			if ((dwDirection & DIR_FORWARD) && (dwDirection & DIR_BACKWARD))
				m_pSkinnedAnimationController->SetAnimationSet(state = Idle);
			else if (dwDirection & DIR_FORWARD)
				m_pSkinnedAnimationController->SetAnimationSet(state = Move_Forward);
			else if (dwDirection & DIR_BACKWARD)
				m_pSkinnedAnimationController->SetAnimationSet(state = Move_Backward);
			else
				m_pSkinnedAnimationController->SetAnimationSet(state = Idle);

		}
		if ((dwDirection & DIR_FORWARD) && (dwDirection & DIR_BACKWARD)) {
			if ((dwDirection & DIR_LEFT) && (dwDirection & DIR_RIGHT))
				m_pSkinnedAnimationController->SetAnimationSet(state = Idle);
			else if (dwDirection & DIR_LEFT)
				m_pSkinnedAnimationController->SetAnimationSet(state = Move_Left);
			else if (dwDirection & DIR_RIGHT)
				m_pSkinnedAnimationController->SetAnimationSet(state = Move_Right);
			else
				m_pSkinnedAnimationController->SetAnimationSet(state = Idle);
		}
	}
	if (!dwDirection)
		m_pSkinnedAnimationController->SetAnimationSet(state = Idle);

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	//if (GetCapture() == m_pFramework->GetHandle())
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		// SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		m_ptOldCursorPos = ptCursorPos;
	}

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f)) {
		if (cxDelta || cyDelta)
		{
			//if (pKeysBuffer[VK_RBUTTON] & 0xF0) Rotate(cyDelta, 0.0f, -cxDelta);
			// else
				Rotate(cyDelta, cxDelta, 0.0f);
		}
		if (dwDirection) Move(dwDirection, 12.25f * fTimeElapsed, true);
	}

	Update(fTimeElapsed);
}
