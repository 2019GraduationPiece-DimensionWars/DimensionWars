#include "stdafx.h"
#include "ResourceManager.h"
#include "AnimationController.h"
#include "Object002_SkinnedFrameObject.h"
#include "Texture.h"
#include "Scene000_BaseScene.h"
#include "Shader000_BaseShader.h"
#include "Shader003_StandardLightingShader.h"


ResourceManager::ResourceManager()
{
	// 사운드시스템 생성 
	FMOD_System_Create(&SoundSystem);

	// 초기화
	FMOD_System_Init(SoundSystem, 10, FMOD_INIT_NORMAL, NULL); // 사운드시스템, 채널 갯수(동시에 재생할 BGM 가짓수), 초기화 상태, 플러그인 드라이버(NULL로 무시)

	FMOD_System_CreateSound(SoundSystem, "Sound\\BGM\\symphonic_metal_vanishing_dream_lost_dream.mp3", FMOD_LOOP_NORMAL | FMOD_2D | FMOD_3D_WORLDRELATIVE | FMOD_3D_INVERSEROLLOFF, 0, &TitleLobbyRoom_Sound);
	FMOD_System_CreateSound(SoundSystem, "Sound\\BGM\\Gluttony Fang.mp3", FMOD_LOOP_NORMAL | FMOD_2D | FMOD_3D_WORLDRELATIVE | FMOD_3D_INVERSEROLLOFF, 0, &BattleScene_Sound);
}

ResourceManager::~ResourceManager()
{
	for (unsigned int i = 0; i < MAX_PLAYER; ++i) {
		if (GrimReaperModel[i]) delete GrimReaperModel[i];
		if (GamblerModel[i]) delete GamblerModel[i];
		if (ElfArcherModel[i]) delete ElfArcherModel[i];
		
		
		//for (unsigned int j = 0; j < nCards; ++j) if (CardModel[i * nCards + j]) delete CardModel[i * nCards + j];
		//for (unsigned int j = 0; j < nArrows; ++j) if (ArrowModel[i * nArrows + j]) delete ArrowModel[i * nArrows + j];
	}

	for (unsigned int i = 0; i < Slash_end - Slash_start; ++i)
		if (SlashWaveModel[i]) delete SlashWaveModel[i];
	for (unsigned int i = 0; i < Card_end - Card_start; ++i) 
		if (CardModel[i]) delete CardModel[i];
	for (unsigned int i = 0; i < Arrow_end - Arrow_start; ++i) 
		if (ArrowModel[i]) delete ArrowModel[i];
	
	for (unsigned int i = 0; i < Potal_end - Potal_start ; ++i)
		if (PortalModel[i]) delete[] PortalModel[i];
}

void ResourceManager::AllModelLoad(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	LoadSlashWave(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	ConsolePrint("검기 모델 로드 완료\n");
	LoadCard(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	ConsolePrint("카드 모델 로드 완료\n");
	LoadArrow(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	ConsolePrint("화살 모델 로드 완료\n");

	for (unsigned int i = 0; i < MAX_PLAYER; ++i){
		LoadGrimReaper(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, i);		
		LoadGambler(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, i);
		LoadElfArcher(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, i);
		ConsolePrint("%d번째 플레이어 모델 로드 완료\n", i + 1);
	}

	Texture *pPortalTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
	pPortalTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Portal.dds", 0);
	BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pPortalTexture, 15, false);
	for (unsigned int i = 0; i < Potal_end - Potal_start ; ++i) {
		if (!PortalModel[i]) {
			PortalModel[i] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Portal.bin", nullptr);
			SkinnedFrameObject* portalObject = PortalModel[i]->m_pModelRootObject->FindFrame("Sphere001");
			portalObject->GetMaterial(0)->SetTexture(pPortalTexture);			
		}
	}
	ConsolePrint("%d개의 포탈 모델 로드 완료\n", Potal_end - Potal_start);
}

void ResourceManager::LoadGrimReaper(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, unsigned int index)
{
	if (!GrimReaperModel[index]) {
		GrimReaperModel[index] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/GrimReaper.bin", nullptr);
		
		Texture *pSkeletonTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pSkeletonTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/Death_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSkeletonTexture, 15, false);
		SkinnedFrameObject* skeleton = GrimReaperModel[index]->m_pModelRootObject->FindFrame("Object001");
		skeleton->GetMaterial(0)->SetTexture(pSkeletonTexture);
	
		Texture *pScytheTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pScytheTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/Death_Scythe_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pScytheTexture, 15, false);
		SkinnedFrameObject* scythe = GrimReaperModel[index]->m_pModelRootObject->FindFrame("Cylinder002");
		scythe->GetMaterial(0)->SetTexture(pScytheTexture);

		Texture *pCapeTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pCapeTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/Death_Cape_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pCapeTexture, 15, false);
		SkinnedFrameObject* cape = GrimReaperModel[index]->m_pModelRootObject->FindFrame("Torus001");
		cape->GetMaterial(0)->SetTexture(pCapeTexture);
	}
}

void ResourceManager::LoadGambler(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, unsigned int index)
{

	Texture *pCardInnerTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
	pCardInnerTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Card_Inner_Base_Color.dds", 0);
	BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pCardInnerTexture, 15, false);

	Texture *pCardOuterTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
	pCardOuterTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Card_Frame_Base_Color.dds", 0);
	BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pCardOuterTexture, 15, false);
	
	SkinnedFrameObject* card = nullptr;
	SkinnedFrameObject* cardFrame = nullptr;
	if (!GamblerModel[index]) {
		GamblerModel[index] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Gambler.bin", nullptr);
		
		Texture *pJacketTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pJacketTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Jacket_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pJacketTexture, 15, false);
		SkinnedFrameObject* Jacket = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Jacket");
		Jacket->GetMaterial(0)->SetTexture(pJacketTexture);

		Texture *pShirtTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pShirtTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Shirts_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pShirtTexture, 15, false);
		SkinnedFrameObject* Shirt = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Shirts");
		Shirt->GetMaterial(0)->SetTexture(pShirtTexture);

		Texture *pHandTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHandTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Hand_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHandTexture, 15, false);
		SkinnedFrameObject* Hand = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Hand");
		Hand->GetMaterial(0)->SetTexture(pHandTexture);

		Texture *pPantsTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pPantsTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Pants_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pPantsTexture, 15, false);
		SkinnedFrameObject* Pants = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Pants");
		Pants->GetMaterial(0)->SetTexture(pPantsTexture);
		
		Texture *pShoesTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pShoesTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Shoes_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pShoesTexture, 15, false);
		SkinnedFrameObject* Shoes = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Shoes");
		Shoes->GetMaterial(0)->SetTexture(pShoesTexture);

		Texture *pNeckTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pNeckTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Neck_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pNeckTexture, 15, false);
		SkinnedFrameObject* Neck = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Neck");
		Neck->GetMaterial(0)->SetTexture(pNeckTexture);

		Texture *pFaceTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pFaceTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Face_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pFaceTexture, 15, false);
		SkinnedFrameObject* Head = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Head");
		Head->GetMaterial(0)->SetTexture(pFaceTexture);

		Texture *pMaskTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pMaskTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Mask_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pMaskTexture, 15, false);
		SkinnedFrameObject* Mask = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Mask");
		Mask->GetMaterial(0)->SetTexture(pMaskTexture);

		Texture *pHairTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHairTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Hair_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHairTexture, 15, false);
		SkinnedFrameObject* hair = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Hair");
		hair->GetMaterial(0)->SetTexture(pHairTexture);

		Texture *pHatBlackTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHatBlackTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Hat_Black_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHatBlackTexture, 15, false);
		SkinnedFrameObject* hat = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Hat_B");
		hat->GetMaterial(0)->SetTexture(pHatBlackTexture);

		Texture *pHatWhiteTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHatWhiteTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Hat_White_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHatWhiteTexture, 15, false);
		SkinnedFrameObject* hat_line = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Hat_W");
		hat_line->GetMaterial(0)->SetTexture(pHatWhiteTexture);
		
		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Inner01");
		card->GetMaterial(0)->SetTexture(pCardInnerTexture);
		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Inner02");
		card->GetMaterial(0)->SetTexture(pCardInnerTexture);
		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Inner03");
		card->GetMaterial(0)->SetTexture(pCardInnerTexture);
		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Inner04");
		card->GetMaterial(0)->SetTexture(pCardInnerTexture);
		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Inner05");
		card->GetMaterial(0)->SetTexture(pCardInnerTexture);
		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Inner06");
		card->GetMaterial(0)->SetTexture(pCardInnerTexture);
		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Inner07");
		card->GetMaterial(0)->SetTexture(pCardInnerTexture);
		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Inner08");
		card->GetMaterial(0)->SetTexture(pCardInnerTexture);

		cardFrame = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Outer01");
		cardFrame->GetMaterial(0)->SetTexture(pCardOuterTexture);
		cardFrame = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Outer02");
		cardFrame->GetMaterial(0)->SetTexture(pCardOuterTexture);
		cardFrame = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Outer03");
		cardFrame->GetMaterial(0)->SetTexture(pCardOuterTexture);
		cardFrame = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Outer04");
		cardFrame->GetMaterial(0)->SetTexture(pCardOuterTexture);
		cardFrame = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Outer05");
		cardFrame->GetMaterial(0)->SetTexture(pCardOuterTexture);
		cardFrame = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Outer06");
		cardFrame->GetMaterial(0)->SetTexture(pCardOuterTexture);
		cardFrame = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Outer07");
		cardFrame->GetMaterial(0)->SetTexture(pCardOuterTexture);
		cardFrame = GamblerModel[index]->m_pModelRootObject->FindFrame("Card_Outer08");
		cardFrame->GetMaterial(0)->SetTexture(pCardOuterTexture);

		/*
		Texture *pBodyTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBodyTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Body_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBodyTexture, 15, false);
		SkinnedFrameObject* Body = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Body");
		strncpy(Body->m_pstrFrameName, "Gambler_Body_Color", 19);
		Body->GetMaterial(0)->SetTexture(pBodyTexture);
		*/
	}

	for (unsigned int i = 0; i < Card_end - Card_start; ++i) {
		if (!CardModel[i]) continue;
		card = CardModel[i]->m_pModelRootObject->FindFrame("Gamble_Card_Inner");
		if (card) card->GetMaterial(0)->SetTexture(pCardInnerTexture);
		cardFrame = CardModel[i]->m_pModelRootObject->FindFrame("Gamble_Card_Frame");
		if (cardFrame) cardFrame->GetMaterial(0)->SetTexture(pCardOuterTexture);
	}
}

void ResourceManager::LoadElfArcher(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, unsigned int index)
{
	Texture *pArrowHeadTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
	pArrowHeadTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ArrowHead_Base_Color.dds", 0);
	BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowHeadTexture, 15, false);

	Texture *pArrowShaftTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
	pArrowShaftTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Arrow_Shaft_Base_Color.dds", 0);
	BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowShaftTexture, 15, false);

	Texture *pArrowFeatherTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
	pArrowFeatherTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Arrow_Feather_Base_Color.dds", 0);
	BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowFeatherTexture, 15, false);

	SkinnedFrameObject* ArrowHead = nullptr;
	SkinnedFrameObject* ArrowShaft = nullptr;
	SkinnedFrameObject* ArrowFeather = nullptr;

	if (!ElfArcherModel[index]) {
		ElfArcherModel[index] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ElfArcher.bin", nullptr);
		
		Texture *pHeadTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHeadTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Head_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHeadTexture, 15, false);
		SkinnedFrameObject* Head = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Head");
		Head->GetMaterial(0)->SetTexture(pHeadTexture);

		Texture *pHairTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHairTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Hair_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHairTexture, 15, false);
		SkinnedFrameObject* hair = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Hair");
		hair->GetMaterial(0)->SetTexture(pHairTexture);

		Texture *pPonyTailTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pPonyTailTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Ponytail_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pPonyTailTexture, 15, false);
		SkinnedFrameObject* PonyTail = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_PonyTail");
		PonyTail->GetMaterial(0)->SetTexture(pPonyTailTexture);

		Texture *pBodyTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBodyTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Body_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBodyTexture, 15, false);
		SkinnedFrameObject* Body = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Body");
		Body->GetMaterial(0)->SetTexture(pBodyTexture);

		Texture *pArmTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pArmTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Arm_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArmTexture, 15, false);
		SkinnedFrameObject* Arm = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arm");
		Arm->GetMaterial(0)->SetTexture(pArmTexture);

		Texture *pArmCoverTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pArmCoverTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Arm_Cover_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArmCoverTexture, 15, false);
		SkinnedFrameObject* ArmCover = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arm_Cover");
		ArmCover->GetMaterial(0)->SetTexture(pArmCoverTexture);

		Texture *pShoulderTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pShoulderTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Shoulder_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pShoulderTexture, 15, false);
		SkinnedFrameObject* Shoulder = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Shoulder");
		Shoulder->GetMaterial(0)->SetTexture(pShoulderTexture);

		Texture *pShirtTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pShirtTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Shirts_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pShirtTexture, 15, false);
		SkinnedFrameObject* Shirts = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Shirts");
		Shirts->GetMaterial(0)->SetTexture(pShirtTexture);

		Texture *pPantsTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pPantsTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Pants_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pPantsTexture, 15, false);
		SkinnedFrameObject* Pants = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Pants");
		Pants->GetMaterial(0)->SetTexture(pPantsTexture);

		Texture *pTightTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pTightTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Tight_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTightTexture, 15, false);
		SkinnedFrameObject* Tight = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Tight");
		Tight->GetMaterial(0)->SetTexture(pTightTexture);

		Texture *pBootsTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBootsTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Boots_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBootsTexture, 15, false);
		SkinnedFrameObject* Boots = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Boots");
		Boots->GetMaterial(0)->SetTexture(pBootsTexture);

		Texture *pVestTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pVestTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Vest_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pVestTexture, 15, false);
		SkinnedFrameObject* Vest = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Vest");
		Vest->GetMaterial(0)->SetTexture(pVestTexture);

		Texture *pBustArmorTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBustArmorTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Bust_Armor_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBustArmorTexture, 15, false);
		SkinnedFrameObject* BustArmor = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Bust_Armor");
		BustArmor->GetMaterial(0)->SetTexture(pBustArmorTexture);

		Texture *pBustArmorRingTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBustArmorRingTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Bust_Armor_Ring_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBustArmorRingTexture, 15, false);
		SkinnedFrameObject* BustArmorRing = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Bust_Armor_Ring");
		BustArmorRing->GetMaterial(0)->SetTexture(pBustArmorRingTexture);

		Texture *pBeltLeatherTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBeltLeatherTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Belt_Leather_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBeltLeatherTexture, 15, false);
		SkinnedFrameObject* BeltLeather = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Belt_Lrather");
		BeltLeather->GetMaterial(0)->SetTexture(pBeltLeatherTexture);

		Texture *pBeltMetalTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBeltMetalTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Belt_Metal_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBeltMetalTexture, 15, false);
		SkinnedFrameObject* BeltMetal = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Belt_Metal");
		BeltMetal->GetMaterial(0)->SetTexture(pBeltMetalTexture);

		Texture *pBowParts1Texture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBowParts1Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Bow_Parts1_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBowParts1Texture, 15, false);
		SkinnedFrameObject* BowParts1 = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Bow_Parts1");
		BowParts1->GetMaterial(0)->SetTexture(pBowParts1Texture);

		Texture *pBowParts2Texture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBowParts2Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Bow_Parts2_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBowParts2Texture, 15, false);
		SkinnedFrameObject* BowParts2 = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Bow_Parts2");
		BowParts2->GetMaterial(0)->SetTexture(pBowParts2Texture);

		Texture *pBowGripTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBowGripTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Bow_Grip_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBowGripTexture, 15, false);
		SkinnedFrameObject* BowGrip = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Bow_Grip");
		BowGrip->GetMaterial(0)->SetTexture(pBowGripTexture);

		Texture *pArrowBagWoodTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pArrowBagWoodTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ArrowBag_Wood_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowBagWoodTexture, 15, false);
		SkinnedFrameObject* ArrowBagWood = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_ArrowBag_Wood");
		ArrowBagWood->GetMaterial(0)->SetTexture(pArrowBagWoodTexture);

		Texture *pArrowBagLeatherTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pArrowBagLeatherTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ArrowBag_Leather_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowBagLeatherTexture, 15, false);
		SkinnedFrameObject* ArrowBagLeather = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_ArrowBag_Leather");
		ArrowBagLeather->GetMaterial(0)->SetTexture(pArrowBagLeatherTexture);
		
		ArrowHead = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Head");
		ArrowHead->GetMaterial(0)->SetTexture(pArrowHeadTexture);
		
		ArrowShaft = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Shaft");
		ArrowShaft->GetMaterial(0)->SetTexture(pArrowShaftTexture);

		ArrowFeather = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Frather");
		ArrowFeather->GetMaterial(0)->SetTexture(pArrowFeatherTexture);

		ArrowHead = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Head001");
		ArrowHead->GetMaterial(0)->SetTexture(pArrowHeadTexture);

		ArrowShaft = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Shaft001");
		ArrowShaft->GetMaterial(0)->SetTexture(pArrowShaftTexture);

		ArrowFeather = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Frather001");
		ArrowFeather->GetMaterial(0)->SetTexture(pArrowFeatherTexture);

		ArrowHead = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Head002");
		ArrowHead->GetMaterial(0)->SetTexture(pArrowHeadTexture);

		ArrowShaft = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Shaft002");
		ArrowShaft->GetMaterial(0)->SetTexture(pArrowShaftTexture);

		ArrowFeather = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Frather002");
		ArrowFeather->GetMaterial(0)->SetTexture(pArrowFeatherTexture);

		ArrowHead = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Head003");
		ArrowHead->GetMaterial(0)->SetTexture(pArrowHeadTexture);

		ArrowShaft = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Shaft003");
		ArrowShaft->GetMaterial(0)->SetTexture(pArrowShaftTexture);

		ArrowFeather = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow_Frather003");
		ArrowFeather->GetMaterial(0)->SetTexture(pArrowFeatherTexture);

		/*
		Texture *pMaskUpperTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pMaskUpperTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Mask_Upper_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pMaskUpperTexture, 15, false);
		SkinnedFrameObject* MaskUpper = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Mask_Upper");
		MaskUpper->GetMaterial(0)->SetTexture(pMaskUpperTexture);

		Texture *pMaskUnderTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pMaskUnderTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Mask_Under_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pMaskUnderTexture, 15, false);
		SkinnedFrameObject* MaskUnder = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Mask_Under");
		MaskUnder->GetMaterial(0)->SetTexture(pMaskUnderTexture);

		Texture *pMaskEyeTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pMaskEyeTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/ElfArcher_Mask_Eye_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pMaskEyeTexture, 15, false);
		SkinnedFrameObject* MaskEye = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Mask_Eye");
		MaskEye->GetMaterial(0)->SetTexture(pMaskEyeTexture);*/
	}

	for (unsigned int i = 0; i < Arrow_end - Arrow_start; ++i) {
		if (!ArrowModel[i]) continue;
			
		ArrowHead = ArrowModel[i]->m_pModelRootObject->FindFrame("Arrow_Head");
		ArrowHead->GetMaterial(0)->SetTexture(pArrowHeadTexture);

		ArrowShaft = ArrowModel[i]->m_pModelRootObject->FindFrame("Arrow_Shaft");
		ArrowShaft->GetMaterial(0)->SetTexture(pArrowShaftTexture);

		ArrowFeather = ArrowModel[i]->m_pModelRootObject->FindFrame("Arrow_Feather");
		ArrowFeather->GetMaterial(0)->SetTexture(pArrowFeatherTexture);
	}
}

void ResourceManager::LoadSlashWave(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	Texture *pSlashWaveTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
	pSlashWaveTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/SlashWave_Base_Color.dds", 0);
	BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSlashWaveTexture, 15, false);
	SkinnedFrameObject* slashWaveObject = nullptr;
	for (unsigned int i = 0; i < Slash_end - Slash_start; ++i){
		if (!SlashWaveModel[i]) {
			SlashWaveModel[i] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SlashWave.bin", nullptr);

			slashWaveObject = SlashWaveModel[i]->m_pModelRootObject->FindFrame("Box001");
			slashWaveObject->GetMaterial(0)->SetTexture(pSlashWaveTexture);
		}
	}
}

void ResourceManager::LoadArrow(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	for (unsigned int i = 0; i < Arrow_end - Arrow_start; ++i)
		if (!ArrowModel[i])
			ArrowModel[i] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Arrow.bin", nullptr);
}

void ResourceManager::LoadCard(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	for (unsigned int i = 0; i < Card_end - Card_start; ++i)
		if (!CardModel[i])
			CardModel[i] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Card.bin", nullptr);
}

LoadedModelInfo * ResourceManager::GetGrimReaperModel(unsigned int index) const
{
	return GrimReaperModel[index];
}

LoadedModelInfo * ResourceManager::GetGamblerModel(unsigned int index) const
{
	return GamblerModel[index];
}

LoadedModelInfo * ResourceManager::GetElfArcherModel(unsigned int index) const
{
	return ElfArcherModel[index];
}

LoadedModelInfo * ResourceManager::GetSlashWaveModel(unsigned int index) const
{
	return SlashWaveModel[index];
}

LoadedModelInfo * ResourceManager::GetCardModel(unsigned int index) const
{
	return CardModel[index];
}

LoadedModelInfo * ResourceManager::GetArrowModel(unsigned int index) const
{
	return ArrowModel[index];
}

LoadedModelInfo * ResourceManager::GetPortalModel(unsigned int index) const
{
	return PortalModel[index];
}

SkinnedFrameObject * ResourceManager::GetSlashWaveObject(unsigned int index) const
{
	return SlashWaveModel[index]->m_pModelRootObject;
}

SkinnedFrameObject * ResourceManager::GetCardObject(unsigned int index) const
{
	return CardModel[index]->m_pModelRootObject;
}

SkinnedFrameObject * ResourceManager::GetArrowObject(unsigned int index) const
{
	return ArrowModel[index]->m_pModelRootObject;
}

SkinnedFrameObject * ResourceManager::GetPortalObject(unsigned int index) const
{
	return PortalModel[index]->m_pModelRootObject;
}

void ResourceManager::PlayTitleLobbyRoomBGM()
{
	FMOD_Channel_Stop(BGM_Channel); // BGM 채널의 현재 음악 중지	
	if (TitleLobbyRoom_Sound && SoundSystem)
		FMOD_System_PlaySound(SoundSystem, TitleLobbyRoom_Sound, NULL, 0, &BGM_Channel);
}

void ResourceManager::PlayBattleBGM()
{
	FMOD_Channel_Stop(BGM_Channel); // BGM 채널의 현재 음악 중지	
	if (BattleScene_Sound && SoundSystem)
		FMOD_System_PlaySound(SoundSystem, BattleScene_Sound, NULL, 0, &BGM_Channel);
}
