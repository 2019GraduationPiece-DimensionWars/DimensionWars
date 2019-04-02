#include "stdafx.h"
#include "Camera002_ThirdPersonCamera.h"
#include "Object100_BasePlayer.h"


ThirdPersonCamera::ThirdPersonCamera(BaseCamera *pCamera)
{
	m_nMode = THIRD_PERSON_CAMERA;
	if (pCamera)
	{
		m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_xmf3Right.y = 0.0f;
		m_xmf3Look.y = 0.0f;
		m_xmf3Right = Vector3::Normalize(m_xmf3Right);
		m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	}
}


ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::Update(XMFLOAT3 & xmf3LookAt, float fTimeElapsed)
{
	if (m_pPlayer) {
		XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Identity();
		XMFLOAT3 xmf3Right = m_pPlayer->GetRightVector();
		XMFLOAT3 xmf3Up = m_pPlayer->GetUpVector();
		XMFLOAT3 xmf3Look = m_pPlayer->GetLookVector();
		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 = xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 = xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 = xmf3Look.z;

		XMFLOAT3 xmf3Offset = Vector3::TransformCoord(m_xmf3Offset, xmf4x4Rotate);
		XMFLOAT3 xmf3Position = Vector3::Add(m_pPlayer->GetPosition(), xmf3Offset);
		XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, m_xmf3Position);
		float fLength = Vector3::Length(xmf3Direction);
		xmf3Direction = Vector3::Normalize(xmf3Direction);


		float fTimeLagScale = (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;

		float fDistance = fLength * fTimeLagScale;
		if (fDistance > fLength) fDistance = fLength;
		if (fLength < 0.01f) fDistance = fLength;
		if (fDistance > 0){
			m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
			SetLookAt(xmf3LookAt);
		}
	}
}

# define _WITH_LEFT_HAND_COORDINATES
void ThirdPersonCamera::SetLookAt(XMFLOAT3 & xmf3LookAt)
{
	XMFLOAT3 xmf3PlayerUp = m_pPlayer->GetUpVector();
	
	XMFLOAT3 Target = { xmf3LookAt.x, xmf3LookAt.y , xmf3LookAt.z + 100};
#ifdef _WITH_LEFT_HAND_COORDINATES
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, Target, xmf3PlayerUp);
#else
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtRH(m_xmf3Position, xmf3LookAt, xmf3PlayerUp);
#endif
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}
