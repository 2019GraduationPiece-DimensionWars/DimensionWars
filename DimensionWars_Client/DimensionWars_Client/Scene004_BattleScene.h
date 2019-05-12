#pragma once
#include "Scene000_BaseScene.h"
class SkyBox;
class DiffuseCubeObject;
class HeightMapTerrain;
class GrimReaperPlayer;
class GamblerPlayer;
class ElfArcherPlayer;

class BattleScene :
	public BaseScene
{
private:
	SkyBox * m_pSkyBox = nullptr;
	DiffuseCubeObject ** m_ppCubeObjects = nullptr;
	unsigned int m_nCubeObjects = 0;
	HeightMapTerrain * m_pTerrain = nullptr;

	bool isBuilded = false;

	//BasePlayer **m_ppOtherPlayers = nullptr;

	GrimReaperPlayer * ReaperObject[MAX_PLAYER];
	GamblerPlayer * GamblerObject[MAX_PLAYER];
	ElfArcherPlayer * ElfObject[MAX_PLAYER];
public:
	BattleScene();
	BattleScene(SceneTag tag, RuntimeFrameWork* pFramework)
	{
		m_Tag = tag;
		m_pFramework = pFramework;
	}
	virtual ~BattleScene();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual bool ProcessInput(UCHAR *pKeysBuffer, float fTimeElapsed = 0.0f);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, BaseCamera *pCamera = nullptr);

	void BuildCube();
	virtual void ProcessPacket(char *ptr) override;
};

