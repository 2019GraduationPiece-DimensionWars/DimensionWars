#pragma once
#include "BaseObject.h"
class CubeObject : public BaseObject
{
public:
	CubeObject();
	~CubeObject();

	float cube_size; // 큐브 크기
	XMFLOAT3 rotate; // 회전정보

};

