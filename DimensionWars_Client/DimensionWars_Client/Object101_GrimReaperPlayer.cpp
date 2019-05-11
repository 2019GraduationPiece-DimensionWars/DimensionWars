#include "stdafx.h"
#include "Object002_SkinnedFrameObject.h"
#include "Object101_GrimReaperPlayer.h"
#include "Camera002_ThirdPersonCamera.h"
#include "AnimationController.h"
#include "Object008_HeightmapTerrain.h"


GrimReaperPlayer::GrimReaperPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	LoadedModelInfo *GrimReaperModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/GrimReaper.bin", nullptr);
	SetChild(GrimReaperModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new AnimationController(pd3dDevice, pd3dCommandList, 1, GrimReaperModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);	
	m_pSkinnedAnimationController->AddAnimationSet(0.0f, 40.0f * keyFrameUnit, "Idle");
	m_pSkinnedAnimationController->AddAnimationSet(42.0f * keyFrameUnit, 62.0f * keyFrameUnit, "OnHit", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(64.0f * keyFrameUnit, 104.0f * keyFrameUnit, "Guard");
	m_pSkinnedAnimationController->AddAnimationSet(106.0f * keyFrameUnit, 146.0f * keyFrameUnit, "Burf", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(148.0f * keyFrameUnit, 208.0f * keyFrameUnit, "Hide Invasion", ANIMATION_TYPE_ONCE);
	// m_pSkinnedAnimationController->AddAnimationSet(210.0 * keyFrameUnit, 330.0 * keyFrameUnit, "Full Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(210.0f * keyFrameUnit, 235.0f * keyFrameUnit, "First Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(235.0f * keyFrameUnit, 271.0f * keyFrameUnit, "Second Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(271.0f * keyFrameUnit, 330.0f * keyFrameUnit, "Third Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(332.0f * keyFrameUnit, 382.0f * keyFrameUnit, "Slash Wave", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(384.0f * keyFrameUnit, 454.0f * keyFrameUnit, "Beheading");
	m_pSkinnedAnimationController->AddAnimationSet(456.0f * keyFrameUnit, 476.0f * keyFrameUnit, "Move Forward");
	m_pSkinnedAnimationController->AddAnimationSet(478.0f * keyFrameUnit, 498.0f * keyFrameUnit, "Move Right");
	m_pSkinnedAnimationController->AddAnimationSet(500.0f * keyFrameUnit, 520.0f * keyFrameUnit, "Move Left");
	m_pSkinnedAnimationController->AddAnimationSet(522.0f * keyFrameUnit, 542.0f * keyFrameUnit, "Move Backward");
	m_pSkinnedAnimationController->AddAnimationSet(544.0f * keyFrameUnit, 574.0f * keyFrameUnit, "Down");
	m_pSkinnedAnimationController->SetAnimationSet(Idle);
	// m_pSkinnedAnimationController->SetTrackSpeed(0, 2.0f);
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


GrimReaperPlayer::~GrimReaperPlayer()
{
}

void GrimReaperPlayer::OnPrepareRender()
{
	BasePlayer::OnPrepareRender();

	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationX(-90.0f), m_xmf4x4ToParent);
}

BaseCamera * GrimReaperPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
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
		SetFriction(1000.0f);
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

void GrimReaperPlayer::Update(float fTimeElapsed) 
{
	BasePlayer::Update(fTimeElapsed);
}

void GrimReaperPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	HeightMapTerrain *pTerrain = (HeightMapTerrain *)m_pPlayerUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	int z = (int)(xmf3PlayerPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z, bReverseQuad) + 0.0f;
	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}
}

void GrimReaperPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	HeightMapTerrain *pTerrain = (HeightMapTerrain *)m_pCameraUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	int z = (int)(xmf3CameraPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z, bReverseQuad) + 15.0f;
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			ThirdPersonCamera *p3rdPersonCamera = (ThirdPersonCamera *)m_pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
		}

		XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();

		ThirdPersonCamera *p3rdPersonCamera = (ThirdPersonCamera *)m_pCamera;
		p3rdPersonCamera->SetLookAt(GetPosition());
	}
	/*
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();

	ThirdPersonCamera *p3rdPersonCamera = (ThirdPersonCamera *)m_pCamera;
	p3rdPersonCamera->SetLookAt(GetPosition());*/
}

void GrimReaperPlayer::ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed)
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
		if (pKeysBuffer[VK_SPACE] & 0xF0) {
			dwDirection |= DIR_UP;
		}
		if (pKeysBuffer['f'] & 0xF0 || pKeysBuffer['F'] & 0xF0) {
			dwDirection |= DIR_DOWN;
		}

		if ((dwDirection & DIR_FORWARD) && (dwDirection & DIR_LEFT))
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Forward);
		if ((dwDirection & DIR_FORWARD) && (dwDirection & DIR_RIGHT))
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Forward);
		if ((dwDirection & DIR_BACKWARD) && (dwDirection & DIR_LEFT))
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Backward);
		if ((dwDirection & DIR_BACKWARD) && (dwDirection & DIR_RIGHT))
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Backward);

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


	if (pKeysBuffer[VK_LBUTTON])
		m_pSkinnedAnimationController->SetAnimationSet(state = First_Attack);


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
		if (cxDelta || cyDelta){
			if (pKeysBuffer[VK_RBUTTON] & 0xF0)
				Rotate(cyDelta, 0.0f, -cxDelta);
			else
				Rotate(cyDelta, cxDelta, 0.0f);
		}
		//if (dwDirection) Move(dwDirection, 300.0f * fTimeElapsed, true);
	}
	this->SetDirectionBit(dwDirection);
	Update(fTimeElapsed);
}

bool GrimReaperPlayer::isCancleEnabled() const
{
	if (m_pSkinnedAnimationController->m_pAnimationSets->GetAnimationSet(state)->m_bEndTrigger) {
		m_pSkinnedAnimationController->m_pAnimationSets->GetAnimationSet(state)->m_bEndTrigger = false;
		return true;
	}
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
