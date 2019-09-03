#include "stdafx.h"
#include "Object013_ArrowObject.h"
#include "AnimationController.h"
#include "RuntimeFrameWork.h"
#include "ResourceManager.h"

ArrowObject::ArrowObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, RuntimeFrameWork * pFramework)
{
	m_pFramework = pFramework;
	SetChild(m_pFramework->GetResource()->GetArrowObject(), true);

	//m_pSkinnedAnimationController = new AnimationController(pd3dDevice, pd3dCommandList, 1, m_pFramework->GetResource()->GetArrowModel());
	//m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);


	SetPosition(XMFLOAT3(0.0f, -200000.0f, 0.0f));

	SetScale(XMFLOAT3(2.5f, 2.5f, 2.5f));
}


ArrowObject::~ArrowObject()
{
}
