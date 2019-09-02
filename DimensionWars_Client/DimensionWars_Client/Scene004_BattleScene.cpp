#include "stdafx.h"
#include "RuntimeFrameWork.h"
#include "Scene004_BattleScene.h"
#include "Object003_SkyBox.h"
#include "Object101_GrimReaperPlayer.h"
#include "Object102_GamblerPlayer.h"
#include "Object103_ElfArcherPlayer.h"
#include "Camera000_BaseCamera.h"
#include "Material.h"
#include "Object007_DiffuseCubeObject.h"
#include "Object008_HeightmapTerrain.h"
#include "ResourceManager.h"
#include "AnimationController.h"	// 로드모델 때문
#include "Object010_SlashWaveObject.h"
#include "Object011_CardObject.h"
#include "Object006_TextureRectObject.h"
#include "Object012_PortalObject.h"
#include "Texture.h"
#include "Shader005_TextureRectangleShader.h"
#include "Object104_DummyPlayer.h"

BattleScene::BattleScene()
{

}


BattleScene::~BattleScene()
{
}

void BattleScene::SendChracterType(int a)
{
	if (a == 0 || a == 1 || a == 2) {
		CSPacket_CharacterType *myMovePacket = reinterpret_cast<CSPacket_CharacterType *>(m_pFramework->GetSendBuf());
		myMovePacket->size = sizeof(CSPacket_CharacterType);
		// 클라이언트가 어느 방향으로 갈 지 키입력 정보를 저장한 비트를 서버로 보내기
		myMovePacket->character_type = a;
		myMovePacket->type = CS_Type::Character_Info;
		m_pFramework->SendPacket(reinterpret_cast<char *>(myMovePacket));
	}
}

void BattleScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{

	//m_pFramework->m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 2, 45); //SuperCobra(17), Gunship(2), Player:Mi24(1), Angrybot()

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature);

	BuildLightsAndMaterials();
	
	XMFLOAT3 xmf3Scale(24.0f, 6.0f, 24.0f);
	XMFLOAT4 xmf4Color(0.1f, 0.1f, 0.1f, 0.5f);
	//m_pTerrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, _T("Texture/HeightMap.raw"), 257, 257, xmf3Scale, xmf4Color);
	//m_pTerrain->SetPosition(-3072.0f, 0.0f, -3072.0f);
	
	m_pFramework->GetResource()->AllModelLoad(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature);

	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature);
	
	//GrimReaperPlayer *pPlayer = new GrimReaperPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	//m_pPlayer = pPlayer;

	//for (int i = 0; i < MAX_PLAYER; ++i)
	//{
	//	Dummy[i] = new GamblerPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	//	m_ppOtherPlayers[i] = Dummy[i];
	//	m_ppOtherPlayers[i]->SetPosition(XMFLOAT3(-100000.0f, -100000.0f, -100000.0f));// 위치 초기화를 하긴 해야되니까 절대 안 그려질 곳에다 짱박아두자.
	//}
	
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		ReaperObject[i] = new GrimReaperPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	//	ReaperObject[i]->SetChild(m_pFramework->GetResource()->GetGrimReaperModel()->m_pModelRootObject, true);
	}
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		GamblerObject[i] = new GamblerPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		//GamblerObject[i]->SetChild(m_pFramework->GetResource()->GetGamblerModel()->m_pModelRootObject, true);
	}
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		ElfObject[i] = new ElfArcherPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		//ElfObject[i]->SetChild(m_pFramework->GetResource()->GetElfArcherModel()->m_pModelRootObject, true);
	}

	cmd = 0;
	//printf("<캐릭터 선택 >\n플레이어 캐릭터 선택을 위해 커맨드를 입력하세요. ( 사신 : 0, 도박사 : 1 ) >>>  ");
	//scanf_s("%d", &cmd);
	switch (cmd) {
	case 0: 
	{
		GrimReaperPlayer *pPlayer = new GrimReaperPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_pPlayer = pPlayer;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			GamblerObject[i] = new GamblerPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
			m_ppOtherPlayers[i] = GamblerObject[i];
			m_ppOtherPlayers[i]->SetPosition(XMFLOAT3(-100000.0f, -100000.0f, -100000.0f));// 위치 초기화를 하긴 해야되니까 절대 안 그려질 곳에다 짱박아두자.
		}
	}
		break;
	case 1:
	{
		GamblerPlayer *pPlayer = new GamblerPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_pPlayer = pPlayer;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			ReaperObject[i] = new GrimReaperPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
			m_ppOtherPlayers[i] = ReaperObject[i];
			m_ppOtherPlayers[i]->SetPosition(XMFLOAT3(-100000.0f, -100000.0f, -100000.0f));// 위치 초기화를 하긴 해야되니까 절대 안 그려질 곳에다 짱박아두자.
		}
	}
		break;
	default:
	{
		ElfArcherPlayer *pPlayer = new ElfArcherPlayer(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
		m_pPlayer = pPlayer;
	}
		break;
	}
	
	m_nCubeObjects = 50;
	m_ppCubeObjects = new TextureCubeObject*[m_nCubeObjects];
	
	for (unsigned int i = 0; i < m_nCubeObjects; ++i) {
		if (i < 5) m_ppCubeObjects[i] = new TextureCubeObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 300);
		else if (i < 10) m_ppCubeObjects[i] = new TextureCubeObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 400);
		else if (i < 20) m_ppCubeObjects[i] = new TextureCubeObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 500);
		else if (i < 30) m_ppCubeObjects[i] = new TextureCubeObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 600);
		else if (i < 50) m_ppCubeObjects[i] = new TextureCubeObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 700);
		
		
	}
	
	slashWave = new SlashWaveObject*[Slash_end - Slash_start];
	for (unsigned int i = 0; i < Slash_end - Slash_start; ++i) {
		slashWave[i] = new SlashWaveObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	}
	
	card = new CardObject*[Card_end - Card_start];
	for (unsigned int i = 0; i < Card_end - Card_start; ++i) {
		card[i] = new CardObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, m_pTerrain, m_pFramework);
	}

	m_ppPotalObjects = new TexturePortalObject*[Potal_end - Potal_start];
	for (unsigned int i = 0; i < Potal_end - Potal_start; ++i) {
		m_ppPotalObjects[i] = new TexturePortalObject(pd3dDevice, pd3dCommandList,m_pFramework->m_pGraphicsRootSignature, 30);
	}

	/*
	Texture *battleImage[ui_texture];

	//0
	battleImage[0] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[0]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/W0.dds", 0);
	//1
	battleImage[1] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[1]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/W1.dds", 0);
	//2
	battleImage[2] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[2]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/W2.dds", 0);
	//3
	battleImage[3] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[3]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/W3.dds", 0);
	//4
	battleImage[4] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[4]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/W4.dds", 0);
	//5
	battleImage[5] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[5]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/W5.dds", 0);
	//6
	battleImage[6] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[6]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/W6.dds", 0);
	//7
	battleImage[7] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[7]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/W7.dds", 0);
	//8
	battleImage[8] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[8]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/W8.dds", 0);
	//9
	battleImage[9] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[9]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/W9.dds", 0);
	// hp
	battleImage[10] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[10]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/HPBar_360x60.dds", 0);
	// sp
	battleImage[11] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[11]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/SPBar_360x60.dds", 0);
	// 레이더
	battleImage[12] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[12]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/Rada.dds", 0);
	// 점수표
	battleImage[13] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[13]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/Score_chart.dds", 0);
	// :
	battleImage[14] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[14]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/Wcolon.dds", 0);
	// 빈 sp
	battleImage[15] = new Texture(1, RESOURCE_TEXTURE2D, 0);
	battleImage[15]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Battle/EmptyBar_360x60.dds", 0);


	


	TextureRectangleShader *pTextureShader = new TextureRectangleShader();
	pTextureShader->CreateShader(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature);
	pTextureShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	for (int i = 0; i < ui_texture; ++i)
	{
		CreateShaderResourceViews(pd3dDevice, pd3dCommandList, battleImage[i], 15, true);
	}


	Material *battleMaterial[ui_texture];

	for (int i = 0; i < ui_texture; ++i)
	{
		battleMaterial[i] = new Material(1);
		battleMaterial[i]->SetTexture(battleImage[i]);
		battleMaterial[i]->SetShader(pTextureShader);

	}

	m_nObjects = 31; //텍스쳐 로드는 45개
	m_battleObjects = new BaseObject*[m_nObjects];

	// 1초단위 숫자 10개 
	for (int i = 0; i < 9; ++i)
	{
		TextureRectObject *battleImageObject = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 30, 30);
		m_battleObjects[i] = battleImageObject;
		m_battleObjects[i]->SetMaterial(0, battleMaterial[i]);
	}
	// 10초단위 숫자 6개 
	for (int i = 9; i < 15; ++i)
	{
		TextureRectObject *battleImageObject1 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 30, 30);
		m_battleObjects[i] = battleImageObject1;
		m_battleObjects[i]->SetMaterial(0, battleMaterial[i-9]);
	}
	// 1분단위 숫자 6개 
	for (int i = 15; i < 21; ++i)
	{
		TextureRectObject *battleImageObject2 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 30, 30);
		m_battleObjects[i] = battleImageObject2;
		m_battleObjects[i]->SetMaterial(0, battleMaterial[i - 15]);
	}
	// hp
	TextureRectObject *battleImageObject3 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 20, 2);
	m_battleObjects[21] = battleImageObject3;
	m_battleObjects[21]->SetMaterial(0, battleMaterial[10]);
	m_battleObjects[21]->SetPosition(0, 0, 0);
	// sp
	TextureRectObject *battleImageObject4 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 20, 2);
	m_battleObjects[22] = battleImageObject4;
	m_battleObjects[22]->SetMaterial(0, battleMaterial[11]);
	// 레이더
	TextureRectObject *battleImageObject5 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 15, 15);
	m_battleObjects[23] = battleImageObject5;
	m_battleObjects[23]->SetMaterial(0, battleMaterial[12]);
	// 점수표
	TextureRectObject *battleImageObject6 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 15, 5);
	m_battleObjects[24] = battleImageObject6;
	m_battleObjects[24]->SetMaterial(0, battleMaterial[13]);
	// :
	TextureRectObject *battleImageObject7 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 5, 5);
	m_battleObjects[25] = battleImageObject7;
	m_battleObjects[25]->SetMaterial(0, battleMaterial[14]);
	// 분
	TextureRectObject *battleImageObject8 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 5, 5);
	m_battleObjects[26] = battleImageObject8;
	m_battleObjects[26]->SetMaterial(0, battleMaterial[5]);
	// 10초대
	TextureRectObject *battleImageObject9 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 5, 5);
	m_battleObjects[27] = battleImageObject9;
	m_battleObjects[27]->SetMaterial(0, battleMaterial[0]);
	// 1초대
	TextureRectObject *battleImageObject10 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 5, 5);
	m_battleObjects[28] = battleImageObject10;
	m_battleObjects[28]->SetMaterial(0, battleMaterial[0]);
	// 빈 hp
	TextureRectObject *battleImageObject11 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 22.5f, 2.5f);
	m_battleObjects[29] = battleImageObject11;
	m_battleObjects[29]->SetMaterial(0, battleMaterial[15]);
	// 빈 sp
	TextureRectObject *battleImageObject12 = new TextureRectObject(pd3dDevice, pd3dCommandList, m_pFramework->m_pGraphicsRootSignature, 22.5f, 2.5f);
	m_battleObjects[30] = battleImageObject12;
	m_battleObjects[30]->SetMaterial(0, battleMaterial[15]);
	
	*/
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void BattleScene::ReleaseObjects()
{
	if (m_pFramework->m_pGraphicsRootSignature) m_pFramework->m_pGraphicsRootSignature->Release();

	if (m_pSkyBox) delete m_pSkyBox;

	ReleaseShaderVariables();
}

void BattleScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();

	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();

	if (m_battleObjects) {
		for (unsigned int i = 0; i < m_nObjects ; ++i)
			if (m_battleObjects[i])
				m_battleObjects[i]->ReleaseUploadBuffers();
	}
}

bool BattleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pFramework->GetActivated() && m_pPlayer) m_pPlayer->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:

		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
	return false;
}

bool BattleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pFramework->GetActivated() && m_pPlayer) m_pPlayer->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		// 키보드를 누르고 있을 경우 최초 한번만 실행.
		if ((lParam & 0x40000000) != 0x40000000) {
			switch (wParam) {
			case 'Z':
				m_pPlayer = ReaperObject[0];
				m_pFramework->m_pCamera = m_pPlayer->GetCamera();
				break;
			case 'X':
				m_pPlayer = GamblerObject[0];
				m_pFramework->m_pCamera = m_pPlayer->GetCamera();
				break;
			case 'C':
				m_pPlayer = ElfObject[0];
				m_pFramework->m_pCamera = m_pPlayer->GetCamera();
				break;
			
			

			}

			
		}
		break;
	case WM_KEYUP:

		break;
	}
	return false;
}

bool BattleScene::ProcessInput(UCHAR * pKeysBuffer, float fTimeElapsed)
{
	if (m_pFramework->GetActivated() && m_pPlayer) m_pPlayer->ProcessInput(pKeysBuffer, fTimeElapsed);
	return false;
}

void BattleScene::AnimateObjects(float fTimeElapsed)
{
	SendMoveDirection();
	SendAnimationInfo();
	if (cmd == 1) {
		m_pPlayer->GetCamera()->SetOffset(XMFLOAT3(0, 150, -350));
		//m_pPlayer->GetCamera()->SetPosition(Vector3::Add(m_pPlayer->GetCamera()->GetPosition(), m_pPlayer->GetCamera()->GetOffset()));
	}
	
	
	BuildCube();
	
	if (m_pPlayer) m_pPlayer->Animate(fTimeElapsed);

	for (int i = 0; i < MAX_PLAYER; ++i)
		if (m_ppOtherPlayers[i]->connected)
			m_ppOtherPlayers[i]->Animate(fTimeElapsed);


	auto reflection = Vector3::Subtract(XMFLOAT3(0.0f, 0.0f, 0.0f), m_pPlayer->GetCamera()->GetLookVector());
	if (m_battleObjects)
		for (unsigned int i = 0; i < m_nObjects; ++i)
			if (m_battleObjects[i]) {
					//m_battleObjects[i]->SetLookAt(m_pPlayer->GetPosition(), m_pPlayer->GetUpVector());
			}

	// 레이더
	
	/*
	//HP바
	m_battleObjects[21]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x-25.0f, m_pPlayer->GetCamera()->GetPosition().y+17.0f, m_pPlayer->GetCamera()->GetPosition().z);
	//SP바
	m_battleObjects[22]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x - 25.0f, m_pPlayer->GetCamera()->GetPosition().y + 14.0f, m_pPlayer->GetCamera()->GetPosition().z );
	
	// 레이더
	m_battleObjects[23]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 29.0f, m_pPlayer->GetCamera()->GetPosition().y + 13.0f, m_pPlayer->GetCamera()->GetPosition().z +350);
	*/
	//m_battleObjects[23]->SetPosition(Vector3::Subtract(m_pPlayer->GetCamera()->GetPosition(), XMFLOAT3(-10, 0, 0)));
	//m_battleObjects[23]->m_xmf4x4World._11 = m_pPlayer->GetCamera()->GetRightVector().x;
	//m_battleObjects[23]->m_xmf4x4World._12 = m_pPlayer->GetCamera()->GetRightVector().y;
	//m_battleObjects[23]->m_xmf4x4World._13 = m_pPlayer->GetCamera()->GetRightVector().z;

	//m_battleObjects[23]->m_xmf4x4World._21 = m_pPlayer->GetCamera()->GetUpVector().x;
	//m_battleObjects[23]->m_xmf4x4World._22 = m_pPlayer->GetCamera()->GetUpVector().y;
	//m_battleObjects[23]->m_xmf4x4World._23 = m_pPlayer->GetCamera()->GetUpVector().z;

	//m_battleObjects[23]->m_xmf4x4World._31 = m_pPlayer->GetCamera()->GetLookVector().x;
	//m_battleObjects[23]->m_xmf4x4World._32 = m_pPlayer->GetCamera()->GetLookVector().y;
	//m_battleObjects[23]->m_xmf4x4World._33 = m_pPlayer->GetCamera()->GetLookVector().z;
	//m_battleObjects[23]->SetPosition(Vector3::Subtract(m_pPlayer->GetCamera()->GetPosition(), XMFLOAT3(-10, 0, 0)));
	//m_battleObjects[23]->SetLookAt(m_pPlayer->GetPosition(), m_pPlayer->GetUpVector());
	//printf("%1.f, %1.f, %1.f\n", m_pPlayer->GetCamera()->GetPosition().x, m_pPlayer->GetCamera()->GetPosition().y, m_pPlayer->GetCamera()->GetPosition().z);
	
	/*
	// 점수표
	m_battleObjects[24]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x, m_pPlayer->GetCamera()->GetPosition().y + 17.0f, m_pPlayer->GetCamera()->GetPosition().z);
	// 콜론
	m_battleObjects[25]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x, m_pPlayer->GetCamera()->GetPosition().y + 12.0f, m_pPlayer->GetCamera()->GetPosition().z);
	// 분
	m_battleObjects[26]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x-3, m_pPlayer->GetCamera()->GetPosition().y + 12.0f, m_pPlayer->GetCamera()->GetPosition().z);
	// 10초대
	m_battleObjects[27]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x+3, m_pPlayer->GetCamera()->GetPosition().y + 12.0f, m_pPlayer->GetCamera()->GetPosition().z);
	// 1초대
	m_battleObjects[28]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x+6, m_pPlayer->GetCamera()->GetPosition().y + 12.0f, m_pPlayer->GetCamera()->GetPosition().z);
	// 빈 바
	m_battleObjects[29]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x - 25.0f, m_pPlayer->GetCamera()->GetPosition().y + 17.0f, m_pPlayer->GetCamera()->GetPosition().z+0.001f);
	m_battleObjects[30]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x - 25.0f, m_pPlayer->GetCamera()->GetPosition().y + 14.0f, m_pPlayer->GetCamera()->GetPosition().z+0.001f);
	*/
	
	/*if (m_nObjects > 5) {
		m_battleObjects[5]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_battleObjects[6]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_battleObjects[7]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_battleObjects[8]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_battleObjects[9]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_battleObjects[10]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_battleObjects[11]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
		m_battleObjects[12]->SetPosition(m_pPlayer->GetCamera()->GetPosition().x + 100.0f, m_pPlayer->GetCamera()->GetPosition().y - 520.0f, m_pPlayer->GetCamera()->GetPosition().z + 100.0f);
	}*/
	/*for (int i = 0; i < m_nCubeObjects; ++i) {
		m_ppCubeObjects[i]->SetPosition(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y - 60, m_pPlayer->GetPosition().z - 50);
	}*/

	// for (unsigned int i = 0; i < Slash_end - Slash_start; ++i) if (slashWave && slashWave[i]) slashWave[i]->Animate(fTimeElapsed);
	//for (unsigned int i = 0; i < Card_end - Card_start; ++i) if (card && card[i]) card[i]->Animate(fTimeElapsed);
}

void BattleScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	m_pFramework->SetGraphicsRootSignature(pd3dCommandList);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);

	if (m_pPlayer) m_pPlayer->Render(pd3dCommandList, pCamera);
	
	for (int i = 0; i < MAX_PLAYER; ++i)
		if (m_ppOtherPlayers[i]->connected) 
			m_ppOtherPlayers[i]->Render(pd3dCommandList, pCamera);


	for (unsigned int i = 0; i < m_nCubeObjects; ++i)
		if (m_ppCubeObjects && m_ppCubeObjects[i])
			m_ppCubeObjects[i]->Render(pd3dCommandList, pCamera);

	for (unsigned int i = 0; i < Slash_end - Slash_start; ++i) 
		if (slashWave && slashWave[i])
			slashWave[i]->Render(pd3dCommandList, pCamera);
	
	for (unsigned int i = 0; i < Card_end - Card_start; ++i) {
		if (card && card[i])
			card[i]->Render(pd3dCommandList, pCamera);
	}

	for (unsigned int i = 0; i < Potal_end - Potal_start; ++i) {
		if (m_ppPotalObjects && m_ppPotalObjects[i]) {
			m_ppPotalObjects[i]->Render(pd3dCommandList, pCamera);
			
		}

	}

	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);


	//if (m_battleObjects) for (unsigned int i = 0; i < m_nObjects; ++i) if (m_battleObjects[i]) m_battleObjects[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < 10; ++i)
	{
	//	m_battleObjects[21 + i]->Render(pd3dCommandList, pCamera);
	}
	
}

void BattleScene::BuildCube()
{
	
	if (cube_build) {
		for (unsigned int i = 0; i < m_nCubeObjects; ++i) {
#ifdef USE_CONSOLE_WINDOW
			// printf("UP Cube [%d] %.2f -  Pos : (%.2f, %.2f, %.2f) / Rot : (%.2f, %.2f, %.2f)\n", i, m_pFramework->cubeSize[i], m_pFramework->cubePos[i].x, m_pFramework->cubePos[i].y, m_pFramework->cubePos[i].z, m_pFramework->cubeRot[i].x, m_pFramework->cubeRot[i].y, m_pFramework->cubeRot[i].z);
#endif
			m_ppCubeObjects[i]->SetPosition(m_pFramework->cubePos[i]);
			//m_ppCubeObjects[i]->SetPosition(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y, m_pPlayer->GetPosition().z);
			m_ppCubeObjects[i]->Rotate(m_pFramework->cubeRot[i].x, m_pFramework->cubeRot[i].y, m_pFramework->cubeRot[i].z);
		}
		cube_build = false;
	}
	if (card_build)
	{
		for (unsigned int i = 0; i < Slash_end - Slash_start; ++i)
			if (slashWave && slashWave[i])
				slashWave[i]->SetPosition(10000, 10000, 10000);
		card_build = false;
	}
	if (slash_build)
	{
		for (unsigned int i = 0; i < Card_end - Card_start; ++i)
			if (card && card[i])
				card[i]->SetPosition(10000, 10000, 10000);
		slash_build = false;
	}
	if(portal_build)
	{
		for (unsigned int i = 0; i < Potal_end - Potal_start; ++i)
			if (m_ppPotalObjects&& m_ppPotalObjects[i])
				m_ppPotalObjects[i]->SetPosition(m_pFramework->potalPos[i]);
		
		portal_build = false;
	}
}

void BattleScene::ProcessPacket(char * ptr)
{
	static bool first_time = true;
	
	switch (ptr[1])
	{
	case SC_Type::LoginOK:
	{
		printf("login\n");
		/*SCPacket_LoginOK *packet = reinterpret_cast<SCPacket_LoginOK *>(ptr);
		m_pFramework->myid = packet->id;
#ifdef USE_CONSOLE_WINDOW
		printf("LOGIN\n");
#endif*/
		break;
	}
	case SC_Type::PutPlayer:
	{
#ifdef USE_CONSOLE_WINDOW
		printf("Put Player\n");
#endif
		SCPacket_PutPlayer *my_packet = reinterpret_cast<SCPacket_PutPlayer *>(ptr);
		unsigned int id = my_packet->id;
		//printf("%d\n", id);
		if (first_time) {
			first_time = false;
			m_pFramework->myid = id;
		}
	
		if (id == m_pFramework->myid) {
			
			m_pPlayer->SetVisible(true);
			m_pPlayer->character_type=my_packet->character_type;
			m_pPlayer->SetPosition((XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z)));
			m_pPlayer->hp = my_packet->hp;
			
			/*if (m_pPlayer->character_type == Character_type::Reaper)
			{
				m_pPlayer = ReaperObject[0];
				m_pFramework->m_pPlayer = m_pPlayer;
				m_pFramework->m_pCamera = m_pPlayer->GetCamera();
				
			}
			if (m_pPlayer->character_type == Character_type::Gamber)
			{
				m_pPlayer = GamblerObject[0];
				m_pFramework->m_pPlayer = m_pPlayer;
				m_pFramework->m_pCamera = m_pPlayer->GetCamera();
			}
			if (m_pPlayer->character_type == Character_type::Elf)
			{
				m_pPlayer = ElfObject[0];
				m_pFramework->m_pPlayer = m_pPlayer;
				m_pFramework->m_pCamera = m_pPlayer->GetCamera();
			}*/
			
			//CSPacket_CharacterType *myTypePacket = reinterpret_cast<CSPacket_CharacterType *>(m_pFramework->GetSendBuf());
			//myTypePacket->size = sizeof(CSPacket_CharacterType);
			//// 클라이언트가 어느 방향으로 갈 지 키입력 정보를 저장한 비트를 서버로 보내기
			//myTypePacket->character_type = m_pPlayer->character_type;
			//myTypePacket->type = CS_Type::Character_Info;
			//m_pFramework->SendPacket(reinterpret_cast<char *>(myTypePacket));
		}
		else if (id < MAX_PLAYER) {
			if (m_ppOtherPlayers[id]) {
				m_ppOtherPlayers[id]->connected = true;
				m_ppOtherPlayers[id]->character_type = my_packet->character_type;
				m_ppOtherPlayers[id]->SetPosition((XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z)));
				m_ppOtherPlayers[id]->hp = my_packet->hp;


				//if (m_ppOtherPlayers[id]->character_type == Character_type::Reaper)
				//{
				//	m_ppOtherPlayers[id] = ReaperObject[1];
				//	//m_pFramework->m_pCamera = m_ppOtherPlayers[id]->GetCamera();

				//}
				//if (m_ppOtherPlayers[id]->character_type == Character_type::Gamber)
				//{
				//	m_ppOtherPlayers[id] = GamblerObject[1];
				//	//m_pFramework->m_pCamera = m_ppOtherPlayers[id]->GetCamera();
				//}
				//if (m_ppOtherPlayers[id]->character_type == Character_type::Elf)
				//{
				//	m_ppOtherPlayers[id] = ElfObject[1];
				//	//m_pFramework->m_pCamera = m_ppOtherPlayers[id]->GetCamera();
				//}
				
			}

		}
		
		break;
	}
	case SC_Type::Position:
	{
		SCPacket_Position *my_packet = reinterpret_cast<SCPacket_Position *>(ptr);
		unsigned short other_id = my_packet->id;
		anime = my_packet->animation_state;
		//printf("%d", anime);
	//	printf("포지션! 서버한테 받기 성공\n");
		if (other_id == m_pFramework->myid) {
			m_pPlayer->SetVisible(true);
			//printf("Your [%d] : (%.1f, %.1f, %.1f)\n", my_packet->id, my_packet->position.x, my_packet->position.y, my_packet->position.z);
			m_pPlayer->SetPosition(my_packet->position);
		}
		else if (other_id < MAX_PLAYER) {
			//m_ppOtherPlayers[other_id]->m_pSkinnedAnimationController->SetAnimationSet(anime);
			m_ppOtherPlayers[other_id]->SetPosition(my_packet->position);
//#ifdef USE_CONSOLE_WINDOW
		}
		else if (other_id>=Card_start&&other_id<Card_end) {
			//printf("도박사 평타 : %1.f, %1.f, %1.f\n", my_packet->position.x, my_packet->position.y, my_packet->position.z);
			// 그려주시고 위치 설정
			if (card && card[other_id - Card_start]) {
				card[other_id - Card_start]->SetPosition(my_packet->position);
				//printf("도박사 평타 받은 후  : %1.f, %1.f, %1.f\n", card[other_id - Card_start]->GetPosition().x, card[other_id - Card_start]->GetPosition().y, card[other_id - Card_start]->GetPosition().z);
			}
		}
		else if (other_id >= Slash_start&&other_id < Slash_end) {
			//printf("검기 : %1.f, %1.f, %1.f\n", my_packet->position.x, my_packet->position.y, my_packet->position.z);
			// 그려주시고 위치 설정
			if (slashWave && slashWave[other_id - Slash_start]) {
				slashWave[other_id - Slash_start]->SetPosition(my_packet->position);
				//printf("검기 받은 후 : %1.f, %1.f, %1.f\n", slashWave[other_id - Slash_start]->GetPosition().x, slashWave[other_id - Slash_start]->GetPosition().y, slashWave[other_id - Slash_start]->GetPosition().z);
			}
		}
		break;
	}
	case SC_Type::Rotate:
	{
		SCPacket_Rotate *my_packet = reinterpret_cast<SCPacket_Rotate *>(ptr);
		unsigned short other_id = my_packet->id;
		obj_rot_x = my_packet->x;
		obj_rot_y = my_packet->y;
		obj_rot_z = my_packet->z;
		
		//printf(" %f, %f, %f\n", my_packet->m_Look.x, my_packet->m_Look.y, my_packet->m_Look.z);
		if (other_id == m_pFramework->myid) {
			m_pPlayer->SetVisible(true);
			m_pPlayer->GetCamera()->Rotate(my_packet->y, my_packet->x, my_packet->z);
			m_pPlayer->SetRight(my_packet->m_Right);
			m_pPlayer->SetUp(my_packet->m_Up);
			m_pPlayer->SetLook(my_packet->m_Look);
			
			//m_battleObjects[21]->SetLookAt(XMFLOAT3(0,0,0), my_packet->m_Up);
			//m_battleObjects[21]->Rotate(0,my_packet->x,0);
			//m_pPlayer->GetCamera()->SetLookAt(my_packet->m_Look);

			//m_pPlayer->SetLook(my_packet->m_Look);
			
			//printf("Your [%d] : (%.1f, %.1f, %.1f)\n", my_packet->id, my_packet->position.x, my_packet->position.y, my_packet->position.z);
		//	m_pPlayer->GetCamera()->Rotate(my_packet->y, my_packet->x, my_packet->z);
			//m_pPlayer->Rotate(my_packet->y, my_packet->x, my_packet->z);

			
		}
		else if (other_id < MAX_PLAYER) {
			m_ppOtherPlayers[other_id]->SetRight(my_packet->m_Right);
			m_ppOtherPlayers[other_id]->SetUp(my_packet->m_Up);
			m_ppOtherPlayers[other_id]->SetLook(my_packet->m_Look);
			
		}
		
		
		break;
	}
	case SC_Type::RemovePlayer:
	{
#ifdef USE_CONSOLE_WINDOW
		printf("Remove\n");
#endif
//		SCPacket_RemovePlayer *my_packet = reinterpret_cast<SCPacket_RemovePlayer *>(ptr);
//		unsigned int other_id = my_packet->id;
//		if (other_id == m_pFramework->myid) m_pPlayer->SetVisible(false);
//		if (other_id < MAX_USER) {
//			if (m_ppOtherPlayers[other_id]) {
//				//m_ppOtherPlayers[other_id]->SetVisible(false);
//				delete m_ppOtherPlayers[other_id];
//				m_ppOtherPlayers[other_id] = nullptr;
//#ifdef USE_CONSOLE_WINDOW
//				printf("Player [%d] Remove from Screen\n", my_packet->id);
//#endif
//			}
//		}
		
		break;
	}
	case SC_Type::Animation:
	{
		SCPacket_Animation *my_packet = reinterpret_cast<SCPacket_Animation*>(ptr);
		unsigned short other_id = my_packet->id;
		anime = my_packet->animation_state;

		if (other_id < MAX_PLAYER) {
			m_ppOtherPlayers[other_id]->m_pSkinnedAnimationController->SetAnimationSet(anime);
			//printf("서버한테 받음 : %d\n", anime);
		}

		break;
	}
	case SC_Type::MapInfo:
	{
		//처리
		
		SCPacket_MapInfo *my_packet = reinterpret_cast<SCPacket_MapInfo *>(ptr);

		unsigned short id = my_packet->id - Cube_start;
		m_pFramework->cubePos[id] = XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z);
		m_pFramework->cubeRot[id] = XMFLOAT3(my_packet->rotate.x, my_packet->rotate.y, my_packet->rotate.z);
		if(cube_build==false)
			cube_build = my_packet->build_cube;
		//printf("%.1f, %.1f, %.1f\n", m_pFramework->cubePos[id].x, m_pFramework->cubePos[id].y, m_pFramework->cubePos[id].z);
		
		break;
	}
	case SC_Type::Potal:
	{
	
		SCPacket_PotalInfo *my_packet = reinterpret_cast<SCPacket_PotalInfo *>(ptr);
		unsigned short id = my_packet->id - Potal_start;
		m_pFramework->potalPos[id] = XMFLOAT3(my_packet->position.x, my_packet->position.y, my_packet->position.z);
		portal_build = my_packet->build_portal;
		break;
	}
	case SC_Type::ProjectTile:
	{
		SCPacket_ProjectTile *my_packet = reinterpret_cast<SCPacket_ProjectTile *>(ptr);
		if (my_packet->projectTile_type == ProjectTile::Card)
		{
			//printf("card (%.1f, %.1f, %.1f)\n", my_packet->position.x, my_packet->position.y, my_packet->position.z);
			unsigned short card_id = my_packet->id - Card_start;
			m_pFramework->cardPos[card_id] = my_packet->position;
			my_packet->position = m_pPlayer[m_pFramework->myid].GetPosition();
			card_build = my_packet->build_projecttile;
		}
		if (my_packet->projectTile_type == ProjectTile::Slash)
		{
			//printf("slash (%.1f, %.1f, %.1f)\n", my_packet->position.x, my_packet->position.y, my_packet->position.z);
			unsigned short slash_id = my_packet->id - Slash_start;
			m_pFramework->slashWavePos[slash_id] = my_packet->position;
			my_packet->position = m_pPlayer[m_pFramework->myid].GetPosition();
			slash_build = my_packet->build_projecttile;
		}
		break;
	}

	case SC_Type::OnHit:
	{
		SCPacket_Hit *my_packet = reinterpret_cast<SCPacket_Hit *>(ptr);
		unsigned short other_id = my_packet->id;
		if (other_id == m_pFramework->myid) {
			m_pPlayer->hp = my_packet->hp;
		}
		else if (other_id < MAX_PLAYER) {
			m_ppOtherPlayers[other_id]->hp = my_packet->hp;
			if (m_ppOtherPlayers[other_id]->character_type == Character_type::Gamber)
			{
				m_ppOtherPlayers[other_id]->m_pSkinnedAnimationController->SetAnimationSet(Gambler::OnHit);
			}
			if (m_ppOtherPlayers[other_id]->character_type == Character_type::Reaper)
			{
				m_ppOtherPlayers[other_id]->m_pSkinnedAnimationController->SetAnimationSet(GrimReaper::OnHit);
			}
			if (m_ppOtherPlayers[other_id]->character_type == Character_type::Elf)
			{
				m_ppOtherPlayers[other_id]->m_pSkinnedAnimationController->SetAnimationSet(ElfArcher::OnHit);
			}
		}
		break;
	}
	case SC_Type::CreateRoom:
	{
		SCPacket_CreateRoom *my_packet = reinterpret_cast<SCPacket_CreateRoom *>(ptr);
		break;
	}
	default:
#ifdef USE_CONSOLE_WINDOW
		printf("Unknown PACKET type [%d]\n", ptr[1]);
#endif
		break;
	}
	
}
