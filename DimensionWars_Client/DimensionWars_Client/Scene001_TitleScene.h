#pragma once
#include "Scene000_BaseScene.h"
#include "Object008_HeightmapTerrain.h"
class SkyBox;
class GrimReaperPlayer;
class GamblerPlayer;

class TitleScene :
	public BaseScene
{
public:
	TitleScene(SceneTag tag, RuntimeFrameWork* pFramework);
	~TitleScene();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual bool ProcessInput(UCHAR *pKeysBuffer, float fTimeElapsed = 0.0f);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, BaseCamera *pCamera = nullptr);

private:
	SkyBox* m_pSkyBox = nullptr;
	GamblerPlayer * sample;
	HeightMapTerrain * m_pTerrain = nullptr;

};

