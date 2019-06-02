#pragma once
#include "Object002_SkinnedFrameObject.h"

class RuntimeFrameWork;

class ProjectileObject :
	public SkinnedFrameObject
{
protected:
	RuntimeFrameWork*			m_pFramework = nullptr;

public:
	ProjectileObject();
	virtual ~ProjectileObject();

	void SetFramework(RuntimeFrameWork* p) { m_pFramework = p; }
};

