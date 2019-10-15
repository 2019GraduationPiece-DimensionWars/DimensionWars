#pragma once
#include "Scene000_BaseScene.h"

constexpr unsigned short ui_texture = 21;

class SkyBox;
class DiffuseCubeObject;
class HeightMapTerrain;
class GrimReaperPlayer;
class GamblerPlayer;
class ElfArcherPlayer;
class SkinnedFrameObject;
class CardObject;
class SlashWaveObject;
class TextureCubeObject;
class PortalObject;
class TexturePortalObject;
class DummyPlayer;
class ArrowObject;

enum Character_type
{
	Reaper=0,
	Gamber=1,
	Elf=2,
};

class BattleScene :
	public BaseScene
{
private:
	SkyBox * m_pSkyBox = nullptr;
	TextureCubeObject ** m_ppCubeObjects = nullptr;  // 변경함
	TexturePortalObject ** m_ppPotalObjects = nullptr;
	//PortalObject ** m_ppPotalObjects = nullptr;
	unsigned int m_nCubeObjects = 0;
	HeightMapTerrain * m_pTerrain = nullptr;

	bool isBuilded = false;

	//BasePlayer **m_ppOtherPlayers = nullptr;

	GrimReaperPlayer * ReaperObject[MAX_PLAYER];
	GamblerPlayer * GamblerObject[MAX_PLAYER];
	ElfArcherPlayer * ElfObject[MAX_PLAYER];
	GamblerPlayer *Dummy[MAX_PLAYER];
	/*GrimReaperPlayer *prepareGrimReaper;
	ElfArcherPlayer *prepareElfArcher;
	GamblerPlayer *prepareGambler;*/

	CardObject** card = nullptr;
	SlashWaveObject** slashWave = nullptr;
	ArrowObject ** arrow = nullptr;
	//SkinnedFrameObject** arrow = nullptr;

	bool cube_build = false;
	bool portal_build = false;
	bool card_build = false;
	bool slash_build = false;
	bool arrow_build = false;

	float obj_rot_x;
	float obj_rot_y;
	float obj_rot_z;

	

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
	unsigned short character_type;

	void SendChracterType(int a);
	int cmd;
	unsigned int anime; // 애니메이션 정보
	unsigned short gametime;
};

