#include "stdafx.h"
#include "Object102_GamblerPlayer.h"
#include "Camera002_ThirdPersonCamera.h"
#include "AnimationController.h"
#include "RuntimeFrameWork.h"
#include "ResourceManager.h"
#include "Object008_HeightmapTerrain.h"


GamblerPlayer::GamblerPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, RuntimeFrameWork * pFramework)
{
	m_pFramework = pFramework;
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	SetChild(m_pFramework->GetResource()->GetGamblerModel()->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new AnimationController(pd3dDevice, pd3dCommandList, 1, m_pFramework->GetResource()->GetGamblerModel());
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pSkinnedAnimationController->AddAnimationSet(0.0f, 40.0f * keyFrameUnit, "Idle");
	m_pSkinnedAnimationController->AddAnimationSet(42.0f * keyFrameUnit, 62.0f * keyFrameUnit, "OnHit", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(64.0f * keyFrameUnit, 104.0f * keyFrameUnit, "Guard", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(106.0f * keyFrameUnit, 131.0f * keyFrameUnit, "Burf", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(133.0f * keyFrameUnit, 173.0f * keyFrameUnit, "Shuffle", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(175.0f * keyFrameUnit, 195.0f * keyFrameUnit, "Idle Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(197.0f * keyFrameUnit, 217.0f * keyFrameUnit, "Multi Shot", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(219.0f * keyFrameUnit, 249.0f * keyFrameUnit, "Wild Card", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(252.0f * keyFrameUnit, 286.0f * keyFrameUnit, "Jump", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(288.0f * keyFrameUnit, 323.0f * keyFrameUnit, "Jump Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(325.0f * keyFrameUnit, 345.0f * keyFrameUnit, "Fall");
	m_pSkinnedAnimationController->AddAnimationSet(347.0f * keyFrameUnit, 367.0f * keyFrameUnit, "Fall Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(369.0f * keyFrameUnit, 399.0f * keyFrameUnit, "Move Forward");
	// m_pSkinnedAnimationController->AddAnimationSet(401.0 * keyFrameUnit, 431.0 * keyFrameUnit, "Move Forward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(401.0f * keyFrameUnit, 416.0f * keyFrameUnit, "Move Forward Attack1", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(416.0f * keyFrameUnit, 431.0f * keyFrameUnit, "Move Forward Attack2", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(433.0f * keyFrameUnit, 463.0f * keyFrameUnit, "Move Right Forward");
	// m_pSkinnedAnimationController->AddAnimationSet(465.0 * keyFrameUnit, 495.0 * keyFrameUnit, "Move Right Forward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(465.0f * keyFrameUnit, 480.0f * keyFrameUnit, "Move Right Forward Attack1", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(480.0f * keyFrameUnit, 495.0f * keyFrameUnit, "Move Right Forward Attack2", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(497.0f * keyFrameUnit, 527.0f * keyFrameUnit, "Move Left Forward");
	// m_pSkinnedAnimationController->AddAnimationSet(529.0 * keyFrameUnit, 559.0 * keyFrameUnit, "Move Left Forward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(529.0f * keyFrameUnit, 544.0f * keyFrameUnit, "Move Left Forward Attack1", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(544.0f * keyFrameUnit, 559.0f * keyFrameUnit, "Move Left Forward Attack2", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(561.0f * keyFrameUnit, 591.0f * keyFrameUnit, "Move Right");
	// m_pSkinnedAnimationController->AddAnimationSet(593.0 * keyFrameUnit, 623.0 * keyFrameUnit, "Move Right Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(593.0f * keyFrameUnit, 608.0f * keyFrameUnit, "Move Right Attack1", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(608.0f * keyFrameUnit, 623.0f * keyFrameUnit, "Move Right Attack2", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(625.0f * keyFrameUnit, 655.0f * keyFrameUnit, "Move Left");
	// m_pSkinnedAnimationController->AddAnimationSet(657.0 * keyFrameUnit, 687.0 * keyFrameUnit, "Move Left Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(657.0f * keyFrameUnit, 672.0f * keyFrameUnit, "Move Left Attack1", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(672.0f * keyFrameUnit, 687.0f * keyFrameUnit, "Move Left Attack2", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(689.0f * keyFrameUnit, 719.0f * keyFrameUnit, "Move Backward");
	// m_pSkinnedAnimationController->AddAnimationSet(721.0 * keyFrameUnit, 751.0 * keyFrameUnit, "Move Backward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(721.0f * keyFrameUnit, 736.0f * keyFrameUnit, "Move Backward Attack1", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(736.0f * keyFrameUnit, 751.0f * keyFrameUnit, "Move Backward Attack2", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(753.0f * keyFrameUnit, 783.0f * keyFrameUnit, "Move Right Backward");
	// m_pSkinnedAnimationController->AddAnimationSet(785.0 * keyFrameUnit, 815.0 * keyFrameUnit, "Move Right Backward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(785.0f * keyFrameUnit, 800.0f * keyFrameUnit, "Move Right Backward Attack1", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(800.0f * keyFrameUnit, 815.0f * keyFrameUnit, "Move Right Backward Attack2", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(817.0f * keyFrameUnit, 847.0f * keyFrameUnit, "Move Left Backward");
	//m_pSkinnedAnimationController->AddAnimationSet(849.0 * keyFrameUnit, 879.0 * keyFrameUnit, "Move Left Backward Attack", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(849.0f * keyFrameUnit, 864.0f * keyFrameUnit, "Move Left Backward Attack1", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(864.0f * keyFrameUnit, 879.0f * keyFrameUnit, "Move Left Backward Attack2", ANIMATION_TYPE_ONCE);
	m_pSkinnedAnimationController->AddAnimationSet(881.0f * keyFrameUnit, 904.0f * keyFrameUnit, "Down", ANIMATION_TYPE_ONCE);
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
	//m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationZ(90.0f), m_xmf4x4ToParent);

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

void GamblerPlayer::Update(float fTimeElapsed)
{
	BasePlayer::Update(fTimeElapsed);
}

void GamblerPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
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

void GamblerPlayer::OnCameraUpdateCallback(float fTimeElapsed)
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
}

void GamblerPlayer::ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed)
{
	DWORD dwDirection = 0;

	if (isCancleEnabled()) {
		// 점프, 추락 처리
		//if (pKeysBuffer[VK_SPACE] & 0xF0) {
		//	dwDirection |= DIR_UP;
		//	m_pSkinnedAnimationController->SetAnimationSet(state = Jump);
		//	// m_pSkinnedAnimationController->SetAnimationSet(state = Idle);
		//}
		//if (pKeysBuffer['f'] & 0xF0 || pKeysBuffer['F'] & 0xF0) {
		//	dwDirection |= DIR_DOWN;
		//	//m_pSkinnedAnimationController->SetAnimationSet(state = Fall);
		//	m_pSkinnedAnimationController->SetAnimationSet(state = Idle);
		//}
		animation_check = true;
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
	if (isShot1) {
		dwDirection = 0;
		switch (state) {
		case Idle:
			m_pSkinnedAnimationController->SetAnimationSet(state = Idle_Attack);
			break;
		case Move_Forward:
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Forward_Attack1);
			dwDirection |= DIR_FORWARD;
			break;
		case Move_Left_Forward:
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Left_Forward_Attack1);
			dwDirection |= (DIR_LEFT | DIR_FORWARD);
			break;
		case Move_Right_Forward:
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Right_Forward_Attack1);
			dwDirection |= (DIR_RIGHT | DIR_FORWARD);
			break;
		case Move_Backward:
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Backward_Attack1);
			dwDirection |= DIR_BACKWARD;
			break;
		case Move_Left_Backward:
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Left_Backward_Attack1);
			dwDirection |= (DIR_LEFT | DIR_BACKWARD);
			break;
		case Move_Right_Backward:
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Right_Backward_Attack1);
			dwDirection |= (DIR_RIGHT | DIR_BACKWARD);
			break;
		case Move_Left:
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Left_Attack1);
			dwDirection |= DIR_LEFT;
			break;
		case Move_Right:
			m_pSkinnedAnimationController->SetAnimationSet(state = Move_Right_Attack1);
			dwDirection |= DIR_RIGHT;
			break;
		case Jump:
			m_pSkinnedAnimationController->SetAnimationSet(state = Jump_Attack);
			break;
		case Fall:
			m_pSkinnedAnimationController->SetAnimationSet(state = Fall_Attack);
			break;
		}
	}
	switch (state) {
	case Idle_Attack:
		dwDirection = 0;
		break;
	case Move_Forward_Attack1:
		dwDirection |= DIR_FORWARD;
		break;
	case Move_Left_Forward_Attack1:
		dwDirection |= (DIR_LEFT | DIR_FORWARD);
		break;
	case Move_Right_Forward_Attack1:
		dwDirection |= (DIR_RIGHT | DIR_FORWARD);
		break;
	case Move_Backward_Attack1:
		dwDirection |= DIR_BACKWARD;
		break;
	case Move_Left_Backward_Attack1:
		dwDirection |= (DIR_LEFT | DIR_BACKWARD);
		break;
	case Move_Right_Backward_Attack1:
		dwDirection |= (DIR_RIGHT | DIR_BACKWARD);
		break;
	case Move_Left_Attack1:
		dwDirection |= DIR_LEFT;
		break;
	case Move_Right_Attack1:
		dwDirection |= DIR_RIGHT;
		break;
	}

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
		if (cxDelta || cyDelta)
		{
			//if (pKeysBuffer[VK_RBUTTON] & 0xF0) Rotate(cyDelta, 0.0f, -cxDelta);
			// else
			Rotate(cyDelta, cxDelta, 0.0f);
		}
		//if (dwDirection) Move(dwDirection, 300.0f * fTimeElapsed, true);
	}
	SetDirectionBit(dwDirection);
	Update(fTimeElapsed);
}

bool GamblerPlayer::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		animation_check = true;
		SendCard();
		isShot1 = true;
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_RBUTTONDOWN:
		animation_check = true;
		if (isCancleEnabled() && state != Guard)
			m_pSkinnedAnimationController->SetAnimationSet(state = Guard);
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
		animation_check = false;
		isShot1 = false;
		::ReleaseCapture();
		break;
	case WM_RBUTTONUP:
		animation_check = false;
		isShot1 = false;
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
	return false;
}

bool GamblerPlayer::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		// 키보드를 누르고 있을 경우 최초 한번만 실행.
		if ((lParam & 0x40000000) != 0x40000000) {
			switch (wParam) {
			case '1':
				animation_check = true;
				isShot1 = true;
				break;
			case '2':
				if (isCancleEnabled()) {
					animation_check = true;
					m_pSkinnedAnimationController->SetAnimationSet(state = Multi_Shot);
				}
				break;
			case '3':
				if (isCancleEnabled()) {
					animation_check = true;
					m_pSkinnedAnimationController->SetAnimationSet(state = Shuffle);
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
					m_pSkinnedAnimationController->SetAnimationSet(state = Wild_Card);
				}
				break;
<<<<<<< HEAD
			case VK_SPACE:
				if (isCancleEnabled()) {
					animation_check = true;
					m_pSkinnedAnimationController->SetAnimationSet(state = Jump);
					SendjumpDirection();
				}
=======
			case '0':
				ResetDir();
>>>>>>> f74d3b03076655f88e8c33148c5f8a79e776000f
				break;
			}
		}
		break;
	case WM_KEYUP:
		animation_check = false;
		switch (wParam) {
		case '1':
			isShot1 = false;
			break;
		}
		break;
	}
	return false;
}

bool GamblerPlayer::isCancleEnabled()
{
	if (m_pSkinnedAnimationController->m_pAnimationSets->GetAnimationSet(state)->m_bEndTrigger &&
		m_pSkinnedAnimationController->m_pAnimationSets->GetAnimationSet(state)->m_nType != ANIMATION_TYPE_LOOP) {
		m_pSkinnedAnimationController->m_pAnimationSets->GetAnimationSet(state)->m_bEndTrigger = false;
		return true;
	}
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


void GamblerPlayer::SendCard()
{

	CSPacket_Attack *myPacket = reinterpret_cast<CSPacket_Attack*>(m_pFramework->GetSendBuf());
	myPacket->size = sizeof(CSPacket_Attack);
	myPacket->type = CS_Type::Attack;
	myPacket->attack_type = Gambler::Idle_Attack;
	myPacket->position = GetPosition();
	m_pFramework->SendPacket(reinterpret_cast<char *>(myPacket));
	//printf("%d\n", myAnimationPacket->animation_state);

}

void GamblerPlayer::SendjumpDirection()
{
	DWORD dwDirection = 0;
	dwDirection |= DIR_UP;

	CSPacket_Move *myMovePacket = reinterpret_cast<CSPacket_Move *>(m_pFramework->GetSendBuf());
	myMovePacket->size = sizeof(CSPacket_Move);
	// 클라이언트가 어느 방향으로 갈 지 키입력 정보를 저장한 비트를 서버로 보내기
	myMovePacket->dir = dwDirection;
	myMovePacket->m_Look = GetLookVector();
	myMovePacket->m_Right = GetRightVector();
	myMovePacket->m_Up = GetUpVector();

	myMovePacket->animation_state = m_pSkinnedAnimationController->m_pAnimationTracks->GetAnimationSet();
	myMovePacket->type = CS_Type::Move;
	m_pFramework->SendPacket(reinterpret_cast<char *>(myMovePacket));

}
