#pragma once
#include "Camera000_BaseCamera.h"
class ThirdPersonCamera :
	public BaseCamera
{
public:
	ThirdPersonCamera(BaseCamera *pCamera);
	virtual ~ThirdPersonCamera();

	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& vLookAt);
};

