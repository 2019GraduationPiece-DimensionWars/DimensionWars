#pragma once
#include "Scene000_BaseScene.h"
class SkyBox;
class TitleScene :
	public BaseScene
{
public:
	TitleScene(SceneTag tag, RuntimeFrameWork* pFramework);
	~TitleScene();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, BaseCamera *pCamera = nullptr);
	virtual bool ProcessInput(UCHAR *pKeysBuffer);

	SkyBox* m_pSkyBox = nullptr;
};

