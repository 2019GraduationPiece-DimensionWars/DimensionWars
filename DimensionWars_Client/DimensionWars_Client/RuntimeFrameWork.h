#pragma once

#include "FrameTimer.h"
#include "Scene000_BaseScene.h"

// 인터페이스이므로 header에 include 한다.
// 헤더파일에는 이 헤더의 클래스에서 사용하는 인터페이스나 이 클래스의 부모 외에는 include하지 않도록 한다.

template<typename Enum> // enum class의 선언형을 알려주며 인자와 대응하는 값을 반환하는 함수
inline constexpr auto GetEnumValueByType(Enum enumerator) noexcept	// enum class E : int {a, b, c}; 일 때,
{																	// auto data = GetEnumValueByType(E::a);
	return static_cast<std::underlying_type_t<enum>>(enummerator);	// data의 형식은 int이고, 값은 0
}
// enum class는 enum을 class로 만든 것이다. 이렇게하면 enum이 자료형이 되어버리므로 같은 enum class밖에 못넣는다.

//#define GetSceneEnumInt(Enum) GetEnumValueType(BaseScene::CurrentScene::Enum)	// 아직 CScene을 선언하지 않았으므로 주석처리

class BaseCamera;
class BasePlayer;
class ResourceManager;

class RuntimeFrameWork final // 프레임워크 클래스는 무언가의 부모 클래스로 만들 생각이 없으므로 final을 붙인다.
{
private:
	HINSTANCE					m_hInstance{ NULL };
	HWND						m_hWnd{ NULL };

	unsigned int				m_nClientWidth;
	unsigned int				m_nClientHeight;

	IDXGIFactory4				*m_pFactory = nullptr;
	IDXGISwapChain3				*m_pSwapChain = nullptr;
	//ID3D12Device				*m_pDevice = nullptr;

	bool						m_bMsaa4xEnable = false;
	unsigned int				m_nMsaa4xQualityLevels = 0;

	static const unsigned int	m_nSwapChainBuffers = 2;
	unsigned int				m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pRtvDescriptorHeap = nullptr;
	unsigned int				m_nRtvDescriptorIncrementSize;

	ID3D12Resource				*m_pDepthStencilBuffer = nullptr;
	ID3D12DescriptorHeap		*m_pDsvDescriptorHeap = nullptr;
	unsigned int				m_nDsvDescriptorIncrementSize;

	//ID3D12CommandAllocator		*m_pCommandAllocator = nullptr;
	ID3D12CommandQueue			*m_pCommandQueue = nullptr;
	//ID3D12GraphicsCommandList	*m_pCommandList = nullptr;

	ID3D12Fence					*m_pFence = nullptr;
	__int64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

	_TCHAR						m_pszFrameRate[70];


	bool Activate;
public:
	RuntimeFrameWork();
	~RuntimeFrameWork();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateDirect3DDevice();
	void CreateCmdQueueAndList();
	
	void CreateRTV();
	void CreateDSV();

	void CreateRtvAndDsvDescHeaps();

	void CreateSwapChain();
	void ChangeSwapChainState();

	void BuildObjects();
	void BuildAllScene();
	void BuildPlayer();

	void ReleaseObjects();

	void ProcessInput();
	void Update();
	void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void ChangeScene(BaseScene::SceneTag tag, bool bDestroy = false);

	BaseScene::SceneTag GetNowTag() const { return m_CurrSceneTag; }
	HWND GetHandle() const { return m_hWnd; }

	void NetworkInitialize();

	void ReadPacket(SOCKET sock);
	void SendPacket(char * clientToServerPacket);

	char* GetSendBuf() { return send_buffer; }
	const bool GetActivated() const { return Activate; }
	ResourceManager*			resourceMgr = nullptr;
	ResourceManager* GetResource() const { return resourceMgr; }

	float cubeSize[50];
	XMFLOAT3 cubePos[50];
	XMFLOAT3 cubeRot[50];
	XMFLOAT3 potalPos[50];

	XMFLOAT3 cardPos[Card_end - Card_start];
	XMFLOAT3 slashWavePos[Slash_end - Slash_start];
	ID3D12RootSignature			*m_pGraphicsRootSignature = nullptr;
	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature); }
	unsigned short nBase_room[6] = { 0, 1, 2, 3, 4, 5 };
	unsigned short nBase_member[6] = { 0,0,0,0,0,0 };
	unsigned short room_num = 0; // 방 그릴 번호
	BaseScene * arrScene[BaseScene::SceneTag::Count];
	ID3D12Device				*m_pDevice = nullptr;
	ID3D12GraphicsCommandList	*m_pCommandList = nullptr;
	ID3D12CommandAllocator		*m_pCommandAllocator = nullptr;
	unsigned short myid;

private:
	FrameTimer m_Timer;
	BasePlayer * m_pPlayer = nullptr;
	BaseCamera * m_pCamera = nullptr;
	//BaseScene * arrScene[BaseScene::SceneTag::Count];
	BaseScene * m_pCurrScene = nullptr;
	BaseScene * m_pPrevScene = nullptr;
	BaseScene::SceneTag m_CurrSceneTag = BaseScene::SceneTag::Game;

	SOCKET mySocket; // 소켓
	char server_ip[17] = "127.0.0.1"; // 서버 IP
									  // 전역변수
	//192.168.20.173
	WSABUF  send_wsabuf;
	char	send_buffer[BUFSIZE];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUFSIZE];
	char	packet_buffer[BUFSIZE];
	DWORD	in_packet_size = 0;
	int		saved_packet_size = 0;
};

