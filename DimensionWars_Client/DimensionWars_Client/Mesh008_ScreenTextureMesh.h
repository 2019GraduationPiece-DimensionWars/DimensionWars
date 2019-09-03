#pragma once
#include "Mesh000_BaseMesh.h"

class ScreenTextureMesh : public BaseMesh
{
public:
	ScreenTextureMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float left = 0.0f, float top = 1.0f, float right = 1.0f, float bottom = 0.0f);
	virtual ~ScreenTextureMesh();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int subset = 0);
};

