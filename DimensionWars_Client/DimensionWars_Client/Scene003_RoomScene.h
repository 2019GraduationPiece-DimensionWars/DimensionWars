#pragma once
#include "Scene000_BaseScene.h"
#include "Object008_HeightmapTerrain.h"

constexpr unsigned short room_texture = 14;
class BaseCamera;
class Object104_DummyPlayer;
class RoomScene :
	public BaseScene
{
public:
	RoomScene();
	RoomScene(SceneTag tag, RuntimeFrameWork* pFramework)
	{
		m_Tag = tag;
		m_pFramework = pFramework;
	}
	virtual ~RoomScene();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual bool ProcessInput(UCHAR *pKeysBuffer, float fTimeElapsed = 0.0f);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, BaseCamera *pCamera = nullptr);
	virtual void ProcessPacket(char *ptr) override;

	void SendRomm_LobbyChange();
	void SendRoomExit();
	void SendOtherCharacter();
	void SendReady();

private:
	bool left_act = false;
	bool right_act = false;
	bool room_exit = false;
	HeightMapTerrain * m_pTerrain = nullptr;
	unsigned short c_type = 0;
	int select_count = 0;
	unsigned int other_id=0; // 다른 플레이어 아이디
	//int x = 0;  그리기 위치 선정할려고 만든거 
	//int y = 0;


	int other_player[6]{4,4,4,4,4,4 }; // 다른 플레이어

	Object104_DummyPlayer * DummyObject[MAX_PLAYER];
	Material *roomMaterial[room_texture];

	int cnt = 0;
	unsigned short ready = 0;
};

