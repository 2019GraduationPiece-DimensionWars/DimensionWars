#include "stdafx.h"
#include "Object002_SkinnedFrameObject.h"
#include "Object101_GrimReaperPlayer.h"
#include "Camera002_ThirdPersonCamera.h"
#include "AnimationController.h"
#include "RuntimeFrameWork.h"
#include "ResourceManager.h"
#include "Object008_HeightmapTerrain.h"


GrimReaperPlayer::GrimReaperPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, RuntimeFrameWork * pFramework)
{
	m_pFramework = pFramework;
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	SetChild(m_pFramework->GetResource()->GetGrimReaperModel()->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new AnimationController(pd3dDevice, pd3dCommandList, 1, m_pFramework->GetResource()->GetGrimReaperModel());
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pSkinnedAnimationController->AddAnimationSet(0.0f, 40.0f * keyFrameUnit, "Idle");
	m_pSkinnedAnimationController->AddAnimationSet(42.0f * keyFrameUnit, 62.0f * keyFrameUnit, "OnHit", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(64.0f * keyFrameUnit, 104.0f * keyFrameUnit, "Guard", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(106.0f * keyFrameUnit, 146.0f * keyFrameUnit, "Burf", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(148.0f * keyFrameUnit, 208.0f * keyFrameUnit, "Hide Invasion", ANIMATION_TYPE_ONCE);
	// m_pSkinnedAnimationController->AddAnimationSet(210.0 * keyFrameUnit, 330.0 * keyFrameUnit, "Full Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(210.0f * keyFrameUnit, 235.0f * keyFrameUnit, "First Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(235.0f * keyFrameUnit, 271.0f * keyFrameUnit, "Second Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(271.0f * keyFrameUnit, 330.0f * keyFrameUnit, "Third Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(332.0f * keyFrameUnit, 382.0f * keyFrameUnit, "Slash Wave", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(384.0f * keyFrameUnit, 454.0f * keyFrameUnit, "Beheading", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(456.0f * keyFrameUnit, 476.0f * keyFrameUnit, "Move Forward");
	m_pSkinnedAnimationController->AddAnimationSet(478.0f * keyFrameUnit, 498.0f * keyFrameUnit, "Move Right");
	m_pSkinnedAnimationController->AddAnimationSet(500.0f * keyFrameUnit, 520.0f * keyFrameUnit, "Move Left");
	m_pSkinnedAnimationController->AddAnimationSet(522.0f * keyFrameUnit, 542.0f * keyFrameUnit, "Move Backward");
	m_pSkinnedAnimationController->AddAnimationSet(544.0f * keyFrameUnit, 574.0f * keyFrameUnit, "Down", ANIMATION_TYPE_ONCE);
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
	if (SecondAttackTrigger) {
		if (m_pSkinnedAnimationController->m_pAnimationSets->GetAnimationSet(First_Attack)->m_bEndTrigger)// 1타 모션이 끝나기 전까지는 2타 시행 안함
			m_pSkinnedAnimationController->SetAnimationSet(state = Second_Attack);
		if (state == Second_Attack) // 1타 모션이 끝나서 2타 모션이 들어왔는지 확인해야 2타 트리거 종료
			SecondAttackTrigger = false;
	}
	else if (ThirdAttackTrigger) {
		if (m_pSkinnedAnimationController->m_pAnimationSets->GetAnimationSet(Second_Attack)->m_bEndTrigger)// 2타 모션이 끝나기 전까지는 3타 시행 안함
			m_pSkinnedAnimationController->SetAnimationSet(state = Third_Attack);
		if (state == Third_Attack) // 2타 모션이 끝나서 3타 모션이 들어왔는지 확인해야 2타 트리거 종료
			ThirdAttackTrigger = false;
	}
	else if (isCancleEnabled()) {
		animation_check = true;
		if (pKeysBuffer[VK_SPACE] & 0xF0) {
			dwDirection |= DIR_UP;
			m_pSkinnedAnimationController->SetAnimationSet(state = Idle);
		}
		if (pKeysBuffer['f'] & 0xF0 || pKeysBuffer['F'] & 0xF0) {
			dwDirection |= DIR_DOWN;
			m_pSkinnedAnimationController->SetAnimationSet(state = Idle);
		}

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
			if (dwDirection & DIR_LEFT)
				m_pSkinnedAnimationController->SetAnimationSet(state = Move_Left);
			else if (dwDirection & DIR_RIGHT)
				m_pSkinnedAnimationController->SetAnimationSet(state = Move_Right);
			else
				m_pSkinnedAnimationController->SetAnimationSet(state = Idle);
		}
		if (!dwDirection)
			m_pSkinnedAnimationController->SetAnimationSet(state = Idle);
	}


	//if (pKeysBuffer[VK_LBUTTON])
	//	m_pSkinnedAnimationController->SetAnimationSet(state = First_Attack);

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	//if (GetCapture() == m_pFramework->GetHandle())
	if (m_pFramework->GetActivated())
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		// SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		m_ptOldCursorPos = ptCursorPos;
	}

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f)) {
		if (cxDelta || cyDelta) {
			/*if (pKeysBuffer[VK_RBUTTON] & 0xF0)
				Rotate(cyDelta, 0.0f, -cxDelta);
			else*/
			SendRotate(cyDelta, cxDelta);
		}
		//if (dwDirection) Move(dwDirection, 300.0f * fTimeElapsed, true);
		//printf("%d,%d\n"cxDelta, cyDelta);
	}
	SetDirectionBit(dwDirection);
	Update(fTimeElapsed);
}

bool GrimReaperPlayer::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		animation_check = true;
		switch (state) {
		case First_Attack:
			SecondAttackTrigger = true;
			break;
		case Second_Attack:
			ThirdAttackTrigger = true;
			break;
		default:
			if (isCancleEnabled()) {
				m_pSkinnedAnimationController->SetAnimationSet(state = First_Attack);
			}
			break;
		}
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_RBUTTONDOWN:
		animation_check = true;
		if (isCancleEnabled() && state != Guard) {
			m_pSkinnedAnimationController->SetAnimationSet(state = Guard);
		}
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
		animation_check = false;
		::ReleaseCapture();
		break;
	case WM_RBUTTONUP:
		animation_check = false;
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
	return false;
}

bool GrimReaperPlayer::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		// 키보드를 누르고 있을 경우 최초 한번만 실행.
		if ((lParam & 0x40000000) != 0x40000000) {
			switch (wParam) {
			case '1':
				animation_check = true;
				switch (state) {
				case First_Attack:
					SecondAttackTrigger = true;
					break;
				case Second_Attack:
					ThirdAttackTrigger = true;
					break;
				default:
					if (isCancleEnabled())
						m_pSkinnedAnimationController->SetAnimationSet(state = First_Attack);
					break;
				}
				break;
			case '2':
				if (isCancleEnabled()) {
					animation_check = true;
					SendSlash();
					m_pSkinnedAnimationController->SetAnimationSet(state = Slash_Wave);
				}
				break;
			case '3':
				if (isCancleEnabled()) {
					animation_check = true;
					m_pSkinnedAnimationController->SetAnimationSet(state = Hide_Invasion);
				}
				break;
			case 'q': case 'Q':
				if (isCancleEnabled()) {
					animation_check = true;
					m_pSkinnedAnimationController->SetAnimationSet(state = Burf);
				}
				break;
			case 'e': case 'E':
				if (isCancleEnabled()) {
					animation_check = true;
					m_pSkinnedAnimationController->SetAnimationSet(state = Beheading);
				}
				break;
			case '0':
				ResetDir();
				break;
			}

		}
		break;
	case WM_KEYUP:
		animation_check = false;

		break;
	}
	return false;
}

bool GrimReaperPlayer::isCancleEnabled()
{
	if (m_pSkinnedAnimationController->m_pAnimationSets->GetAnimationSet(state)->m_bEndTrigger &&
		m_pSkinnedAnimationController->m_pAnimationSets->GetAnimationSet(state)->m_nType != ANIMATION_TYPE_LOOP) {
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


void GrimReaperPlayer::SendSlash()
{
	CSPacket_Attack *myPacket = reinterpret_cast<CSPacket_Attack*>(m_pFramework->GetSendBuf());
	myPacket->size = sizeof(CSPacket_Attack);
	myPacket->type = CS_Type::Attack;
	myPacket->attack_type = GrimReaper::Slash_Wave;
	myPacket->position = GetPosition();
	m_pFramework->SendPacket(reinterpret_cast<char *>(myPacket));
}

