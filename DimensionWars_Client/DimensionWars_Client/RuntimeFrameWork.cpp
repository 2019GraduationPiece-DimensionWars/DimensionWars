#include "stdafx.h"
#include "RuntimeFrameWork.h"
#include "Scene001_TitleScene.h"
#include "Scene002_LobbyScene.h"
#include "Scene003_RoomScene.h"
#include "Scene004_BattleScene.h"
#include "Camera000_BaseCamera.h"
#include "Object100_BasePlayer.h"

#include "ResourceManager.h"

template<typename T>
T GetUserDataPtr(HWND hWnd)
{
	return reinterpret_cast<T>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

void SetUserDataPtr(HWND hWnd, LPVOID ptr)
{
	LONG_PTR result = ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
}

RuntimeFrameWork::RuntimeFrameWork()
{
	m_pFactory = nullptr;
	m_pSwapChain = nullptr;
	m_pDevice = nullptr;

	for (int i = 0; i < m_nSwapChainBuffers; ++i) m_ppSwapChainBackBuffers[i] = nullptr;
	m_nSwapChainBufferIndex = 0;

	m_pCommandAllocator = nullptr;
	m_pCommandQueue = nullptr;
	m_pCommandList = nullptr;

	m_pRtvDescriptorHeap = nullptr;
	m_pDsvDescriptorHeap = nullptr;

	m_nRtvDescriptorIncrementSize = 0;
	m_nDsvDescriptorIncrementSize = 0;

	m_hFenceEvent = nullptr;
	m_pFence = nullptr;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_nClientWidth = FRAME_BUFFER_WIDTH;
	m_nClientHeight = FRAME_BUFFER_HEIGHT;

	m_pCurrScene = nullptr;
	m_pCamera = nullptr;
	m_pPlayer = nullptr;
//#ifdef USE_CONSOLE_WINDOW
//	printf(" Server IP를 입력하세요. >> ");
//	scanf_s("%s", server_ip, unsigned int(sizeof(server_ip)));
//#endif

	_tcscpy_s(m_pszFrameRate, _T("Dimension Wars - 차원대전 - "));
}

RuntimeFrameWork::~RuntimeFrameWork()
{

}

bool RuntimeFrameWork::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	resourceMgr = new ResourceManager();
	

	// 클래스와 윈도우 프로시저 연결
	::SetUserDataPtr(m_hWnd, this);

	CreateDirect3DDevice();
	CreateCmdQueueAndList();
	CreateRtvAndDsvDescHeaps();
	CreateSwapChain();
	CreateDSV();

	// CreateOffScreenRenderTargetViews();

	BuildAllScene();

	ChangeScene(BaseScene::SceneTag::Title);
	//ChangeScene(BaseScene::SceneTag::Game);

	// 네트워크 초기화
	//NetworkInitialize();
	BuildObjects();

	return (m_hWnd != NULL);
}

void RuntimeFrameWork::OnDestroy()
{
	ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

	if (m_pDepthStencilBuffer) 
		m_pDepthStencilBuffer->Release();
	if (m_pDsvDescriptorHeap) 
		m_pDsvDescriptorHeap->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++) 
		if (m_ppSwapChainBackBuffers[i]) 
			m_ppSwapChainBackBuffers[i]->Release();
	if (m_pRtvDescriptorHeap) 
		m_pRtvDescriptorHeap->Release();

	if (m_pCommandAllocator) 
		m_pCommandAllocator->Release();
	if (m_pCommandQueue) 
		m_pCommandQueue->Release();
	if (m_pCommandList) 
		m_pCommandList->Release();

	if (m_pFence)
		m_pFence->Release();

	m_pSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pSwapChain) 
		m_pSwapChain->Release();
	if (m_pDevice) 
		m_pDevice->Release();
	if (m_pFactory) 
		m_pFactory->Release();

	if (resourceMgr) 
		delete resourceMgr;
#if defined(_DEBUG)
	IDXGIDebug1	*pdxgiDebug = nullptr;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void **)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void RuntimeFrameWork::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug *pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&pd3dDebugController);
	if (pd3dDebugController) {
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void **)&m_pFactory);

	IDXGIAdapter1 *pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pFactory->EnumAdapters1(i, &pd3dAdapter); ++i) {
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) 
			continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pDevice))) 
			break;
	}

	if (!pd3dAdapter) {
		m_pFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pDevice);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, 
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	hResult = m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pFence);
	for (UINT i = 0; i < m_nSwapChainBuffers; ++i) 
		m_nFenceValues[i] = 0;

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);


	// 악 악 악 
	::gnCbvSrvDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); // 아아아악


	if (pd3dAdapter) 
		pd3dAdapter->Release();
}

void RuntimeFrameWork::CreateCmdQueueAndList()
{
	HRESULT hResult;

	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hResult = m_pDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pCommandQueue);

	hResult = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pCommandAllocator);

	hResult = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void **)&m_pCommandList);
	
	hResult = m_pCommandList->Close();
}

void RuntimeFrameWork::CreateRTV()
{
	D3D12_CPU_DESCRIPTOR_HANDLE RtvCPUDescriptorHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) {
		m_pSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void **)&m_ppSwapChainBackBuffers[i]);
		m_pDevice->CreateRenderTargetView(m_ppSwapChainBackBuffers[i], NULL, RtvCPUDescriptorHandle);
		RtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void RuntimeFrameWork::CreateDSV()
{
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = m_nClientWidth;
	resourceDesc.Height = m_nClientHeight;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	resourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties;
	::ZeroMemory(&heapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	m_pDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pDepthStencilBuffer);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	::ZeroMemory(&dsvDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CPU_DESCRIPTOR_HANDLE DsvCPUDescriptorHandle = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &dsvDesc, DsvCPUDescriptorHandle);
}

void RuntimeFrameWork::CreateRtvAndDsvDescHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	::ZeroMemory(&descriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	descriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;
	HRESULT hResult =
		m_pDevice->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pRtvDescriptorHeap);
	m_nRtvDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult =
		m_pDevice->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pDsvDescriptorHeap);
	m_nDsvDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void RuntimeFrameWork::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nClientWidth = rcClient.right - rcClient.left;
	m_nClientHeight = rcClient.bottom - rcClient.top;

#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1 **)&m_pdxgiSwapChain);
#else
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	::ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = m_nSwapChainBuffers;
	swapChainDesc.BufferDesc.Width = m_nClientWidth;
	swapChainDesc.BufferDesc.Height = m_nClientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult = m_pFactory->CreateSwapChain(m_pCommandQueue, &swapChainDesc, (IDXGISwapChain **)&m_pSwapChain);
#endif
	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	hResult = m_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRTV();
#endif
}

void RuntimeFrameWork::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	m_pSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	// 렌더타겟의 상태. 해상도는 여기서 바꿔주도록 하자
	DXGI_MODE_DESC targetParameters;
	targetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	targetParameters.Width = m_nClientWidth;
	targetParameters.Height = m_nClientHeight;
	targetParameters.RefreshRate.Numerator = 60;
	targetParameters.RefreshRate.Denominator = 1;
	targetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	targetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pSwapChain->ResizeTarget(&targetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppSwapChainBackBuffers[i]) m_ppSwapChainBackBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nClientWidth, m_nClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	CreateRTV();
}

void RuntimeFrameWork::BuildObjects()
{	
	m_pCommandList->Reset(m_pCommandAllocator, NULL);

	
	//m_pCurrScene->BuildObjects(m_pDevice, m_pCommandList);	// 루트 시그니처 생성
	m_pGraphicsRootSignature = m_pCurrScene->CreateGraphicsRootSignature(m_pDevice);
	m_pCurrScene->CreateCbvSrvDescriptorHeaps(m_pDevice, m_pCommandList, 2, 256);
	//GetResource()->AllModelLoad(m_pDevice, m_pCommandList, m_pGraphicsRootSignature);

	
	arrScene[BaseScene::SceneTag::Title]->BuildObjects(m_pDevice, m_pCommandList);
	arrScene[BaseScene::SceneTag::Lobby]->BuildObjects(m_pDevice, m_pCommandList);
	arrScene[BaseScene::SceneTag::Room]->BuildObjects(m_pDevice, m_pCommandList);
	arrScene[BaseScene::SceneTag::Game]->BuildObjects(m_pDevice, m_pCommandList); // 플레이어 선생성
	
	
	
	//m_pPlayer = arrScene[BaseScene::SceneTag::Title]->m_pPlayer;
	m_pPlayer = arrScene[BaseScene::SceneTag::Game]->m_pPlayer;
	m_pCamera = m_pPlayer->GetCamera();

	m_pCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

	for (int i = 0; i < BaseScene::SceneTag::Count ; ++i)
		if (arrScene[i]) arrScene[i]->ReleaseUploadBuffers();
	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();
	
	m_Timer.Reset();
}

void RuntimeFrameWork::BuildAllScene()
{
	arrScene[BaseScene::SceneTag::Title] = new TitleScene(BaseScene::SceneTag::Title, this);
	arrScene[BaseScene::SceneTag::Lobby] = new LobbyScene(BaseScene::SceneTag::Lobby, this);
	arrScene[BaseScene::SceneTag::Room] = new RoomScene(BaseScene::SceneTag::Room, this);
	arrScene[BaseScene::SceneTag::Game] = new BattleScene(BaseScene::SceneTag::Game, this);
}

void RuntimeFrameWork::BuildPlayer()
{
}

void RuntimeFrameWork::ReleaseObjects()
{
	if (m_pPlayer) m_pPlayer->Release();

	m_pCurrScene = nullptr;
	for (int i = 0; i < BaseScene::SceneTag::Count; ++i)
		if (arrScene[i]) {
			arrScene[i]->ReleaseObjects();
			delete arrScene[i];
			arrScene[i] = nullptr;
		}
}

void RuntimeFrameWork::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pCurrScene) bProcessedByScene = m_pCurrScene->ProcessInput(pKeysBuffer, m_Timer.GetTimeElapsed());
}

void RuntimeFrameWork::Update()
{
	float fTimeElapsed = m_Timer.GetTimeElapsed();

	if (m_pCurrScene) m_pCurrScene->AnimateObjects(fTimeElapsed);


	
}

void RuntimeFrameWork::FrameAdvance()
{
	m_Timer.Tick(60.0f);

	/*BattleScene *pScene = reinterpret_cast<BattleScene*>(arrScene[BaseScene::SceneTag::Game]);

	

	pScene->BuildCube();*/
	ProcessInput();
	
    Update();
	
	HRESULT hResult = m_pCommandAllocator->Reset();
	hResult = m_pCommandList->Reset(m_pCommandAllocator, NULL);

	D3D12_RESOURCE_BARRIER resourceBarrier;
	::ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = m_ppSwapChainBackBuffers[m_nSwapChainBufferIndex];
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pCommandList->ResourceBarrier(1, &resourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE RtvCPUDescriptorHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	RtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	float pfClearColor[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	m_pCommandList->ClearRenderTargetView(RtvCPUDescriptorHandle, pfClearColor, 0, NULL); // Colors::Azure

	D3D12_CPU_DESCRIPTOR_HANDLE DsvCPUDescriptorHandle = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pCommandList->ClearDepthStencilView(DsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pCommandList->OMSetRenderTargets(1, &RtvCPUDescriptorHandle, TRUE, &DsvCPUDescriptorHandle);

	if (m_pCurrScene) m_pCurrScene->Render(m_pCommandList, m_pCamera);

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pCommandList->ResourceBarrier(1, &resourceBarrier);

	hResult = m_pCommandList->Close();

	ID3D12CommandList *CommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, CommandLists);

	WaitForGpuComplete();

#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
#else
#ifdef _WITH_SYNCH_SWAPCHAIN
	m_pdxgiSwapChain->Present(1, 0);
#else
	m_pSwapChain->Present(0, 0);
#endif
#endif

	// m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	MoveToNextFrame();

	m_Timer.GetFrameRate(m_pszFrameRate + 24, 50);
	size_t nLength = _tcslen(m_pszFrameRate);
	XMFLOAT3 xmf3Position = m_pPlayer->GetPosition();
	_stprintf_s(m_pszFrameRate + nLength, 70 - nLength, _T("(%.1f, %.1f, %.1f)"), xmf3Position.x, xmf3Position.y, xmf3Position.z);
	::SetWindowText(m_hWnd, m_pszFrameRate);

}

void RuntimeFrameWork::WaitForGpuComplete()
{
	const unsigned long long nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pCommandQueue->Signal(m_pFence, nFenceValue);

	if (m_pFence->GetCompletedValue() < nFenceValue) {
		hResult = m_pFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void RuntimeFrameWork::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	//m_nSwapChainBufferIndex = (m_nSwapChainBufferIndex + 1) % m_nSwapChainBuffers;

	unsigned long long nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pCommandQueue->Signal(m_pFence, nFenceValue);

	if (m_pFence->GetCompletedValue() < nFenceValue) {
		hResult = m_pFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void RuntimeFrameWork::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pCurrScene) m_pCurrScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		//::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void RuntimeFrameWork::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pCurrScene) m_pCurrScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (m_CurrSceneTag == BaseScene::SceneTag::Title)
				ChangeScene(BaseScene::SceneTag::Game);
			else
				ChangeScene(BaseScene::SceneTag::Title);
			break;
		case VK_RETURN:
			break;
		case VK_F1:
		case VK_F2:
		case VK_F3:
			// m_pCamera = m_pPlayer->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
			break;
		case VK_F9:
			ChangeSwapChainState();
			break;
		case VK_SPACE:
			//m_pCamera = m_pPlayer->GetCamera();
			if (m_CurrSceneTag==BaseScene::SceneTag::Title)
			{
				ChangeScene(BaseScene::SceneTag::Lobby);
			}
			else if (m_CurrSceneTag == BaseScene::SceneTag::Lobby)
			{
				ChangeScene(BaseScene::SceneTag::Room);
				
			}
			else if (m_CurrSceneTag == BaseScene::SceneTag::Room)
			{
				ChangeScene(BaseScene::SceneTag::Lobby);
			}
			
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT RuntimeFrameWork::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE) {
			// 비활성화
			m_Timer.Stop();
			Activate = false;
		}
		else {
			// 활성화
			Activate = true;
			m_Timer.Start();
		}
		break;

	}
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);
}

LRESULT RuntimeFrameWork::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	RuntimeFrameWork* self = ::GetUserDataPtr<RuntimeFrameWork*>(hWnd);	// static 함수랑 연결하려고 선언한 포인터. 클래스를 정적함수에 등록하기 위한 꼼수
	if (!self) 
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);	// 메시지 처리를 OS에게 넘긴다.

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:

	case WM_MOUSEMOVE:
		self->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		self->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_PAINT: 
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hWnd, &ps);
		::EndPaint(hWnd, &ps); // 그리고 지운다.
	}
		break;
	case WM_SOCKET:
		if (WSAGETSELECTERROR(lParam)) {
			closesocket((SOCKET)wParam);
			exit(-1);
			break;
		}
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			self->ReadPacket((SOCKET)wParam);
			break;
		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			exit(-1);
			break;
		}
		break;
	case WM_DESTROY:
		::SetUserDataPtr(hWnd, NULL);
		::PostQuitMessage(0);
		break;

	default:
		return self->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
	}
	return 0;
}

void RuntimeFrameWork::ChangeScene(BaseScene::SceneTag tag, bool bDestroy)
{
	if (BaseScene::SceneTag::Count > tag) {
		m_pPrevScene = m_pCurrScene;
		m_pCurrScene = arrScene[m_CurrSceneTag = tag];
	}
}

void RuntimeFrameWork::NetworkInitialize()
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
#ifdef USE_CONSOLE_WINDOW
		::puts("Error - Can't load 'winsock.dll' file");
#endif
		::PostQuitMessage(0);
	}

	// socket()
	// client_sock = socket(AF_INET, SOCK_STREAM, 0);
	mySocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);//WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mySocket == INVALID_SOCKET) {
#ifdef USE_CONSOLE_WINDOW
		::printf("Error - Invalid socket\n");
#endif
		::PostQuitMessage(0);
	}
#ifdef USE_CONSOLE_WINDOW
	printf("Client : %d\n", static_cast<int>(mySocket));
#endif

	// 서버정보 객체설정	
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	// serveraddr.sin_addr.s_addr = inet_addr(server_ip);
	serveraddr.sin_addr.S_un.S_addr = inet_addr(server_ip);
	serveraddr.sin_port = htons(SERVERPORT);

	// connect()
	// if (connect(client_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
	if (WSAConnect(mySocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
#ifdef USE_CONSOLE_WINDOW
		printf("%s IP Error - Fail to connect\n", server_ip);
#endif
		closesocket(mySocket);
		WSACleanup();
		::PostQuitMessage(0);
	}

	WSAAsyncSelect(mySocket, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ); // m_hWnd 초기화도 안하고 생성자에서 이걸 부르고 있었으니 당연히 안되지...

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUFSIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUFSIZE;
#ifdef USE_CONSOLE_WINDOW
	puts("Server Connected");
#endif

	//ReadPacket((SOCKET)mySocket);
}


void RuntimeFrameWork::ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
#ifdef USE_CONSOLE_WINDOW
		printf("Recv Error [%d]\n", err_code);
#endif
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			m_pCurrScene->ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void RuntimeFrameWork::SendPacket(char * clientToServerPacket)
{
	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = clientToServerPacket[0];	// size
	DWORD ioByte = 0;

	int nByteCheck = WSASend(mySocket, &send_wsabuf, 1, &ioByte, 0, NULL, NULL);
#ifdef USE_CONSOLE_WINDOW
	if (nByteCheck) printf("Error while sending packet [%d]", WSAGetLastError());
#endif
}