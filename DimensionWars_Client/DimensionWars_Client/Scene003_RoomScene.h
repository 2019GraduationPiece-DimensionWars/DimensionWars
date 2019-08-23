#pragma once
#include "Scene000_BaseScene.h"
#include "Object008_HeightmapTerrain.h"

constexpr unsigned short room_texture = 13;
class BaseCamera;

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

private:
	bool left_act = false;
	bool right_act = false;
	bool room_exit = false;
	HeightMapTerrain * m_pTerrain = nullptr;
	
};

