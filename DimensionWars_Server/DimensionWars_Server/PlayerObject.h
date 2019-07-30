#pragma once
#include "BaseObject.h"
class PlayerObject : public BaseObject
{
public:
	PlayerObject();
	~PlayerObject();


	void Move(unsigned char dir);

	
	unsigned short character_info;
	XMFLOAT3 m_Right;
	XMFLOAT3 m_Up;
	XMFLOAT3 m_Look;
	float hp = 100;
	unsigned char dir;
	XMFLOAT3 xmf3Shift = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float fDistance = 10.0f;

	bool jump_check = false;

	int character_type = 99;
};

