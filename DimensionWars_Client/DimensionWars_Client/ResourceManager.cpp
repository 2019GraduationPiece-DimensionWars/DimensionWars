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
}

ResourceManager::~ResourceManager()
{
	if (GrimReaperModel) delete GrimReaperModel;
	if (GamblerModel) delete GamblerModel;
	if (ElfArcherModel) delete ElfArcherModel;
	
	if (SlashWaveModel) delete SlashWaveModel;
	if (CardModel) delete CardModel;
	if (ArrowModel) delete ArrowModel;
	if (PortalModel) delete PortalModel;
}

void ResourceManager::AllModelLoad(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	for (unsigned int i = 0; i < MAX_PLAYER; ++i){
		LoadGrimReaper(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, i);		
		LoadGambler(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, i);
		LoadElfArcher(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, i);
		printf("%d번째 플레이어 모델 로드 완료\n", i + 1);
	}

	for (unsigned int i = 0; i < Potal_end - Potal_start ; ++i) {
		if (!PortalModel[i]) {
			PortalModel[i] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Portal.bin", nullptr);
			Texture *pPortalTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
			pPortalTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Portal.dds", 0);
			BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pPortalTexture, 15, false);
			SkinnedFrameObject* portalObject = PortalModel[i]->m_pModelRootObject->FindFrame("Sphere001");
			strncpy(portalObject->m_pstrFrameName, "Portal", 8);
			portalObject->GetMaterial(0)->SetTexture(pPortalTexture);			
		}
	}
	printf("%d개의 포탈 모델 로드 완료\n", Potal_end - Potal_start);
}

void ResourceManager::LoadGrimReaper(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, unsigned int index)
{
	if (!GrimReaperModel[index]) {
		GrimReaperModel[index] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/GrimReaper.bin", nullptr);
		
		Texture *pSkeletonTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pSkeletonTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/Death_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSkeletonTexture, 15, false);
		SkinnedFrameObject* skeleton = GrimReaperModel[index]->m_pModelRootObject->FindFrame("Object001");
		strncpy(skeleton->m_pstrFrameName, "GrimReaper_Base_Color", 23);
		skeleton->GetMaterial(0)->SetTexture(pSkeletonTexture);
	
		Texture *pScytheTexture = new Texture(1, RESOURCE_TEXTURE2DARRAY, 0);
		pScytheTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/Death_Scythe_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pScytheTexture, 15, false);
		SkinnedFrameObject* scythe = GrimReaperModel[index]->m_pModelRootObject->FindFrame("Cylinder002");
		strncpy(scythe->m_pstrFrameName, "GrimReaper_Scythe_Base_Color", 30);
		scythe->GetMaterial(0)->SetTexture(pScytheTexture);

		Texture *pCapeTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pCapeTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/Death_Cape_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pCapeTexture, 15, false);
		SkinnedFrameObject* cape = GrimReaperModel[index]->m_pModelRootObject->FindFrame("Torus001");
		strncpy(cape->m_pstrFrameName, "GrimReaper_Cape_Base_Color", 27);
		cape->GetMaterial(0)->SetTexture(pCapeTexture);
	}

	if (!SlashWaveModel[index]) {
		SlashWaveModel[index] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SlashWave.bin", nullptr);
		Texture *pSlashWaveTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pSlashWaveTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/SlashWave_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSlashWaveTexture, 15, false);
		SkinnedFrameObject* slashWaveObject = SlashWaveModel[index]->m_pModelRootObject->FindFrame("Box001");
		strncpy(slashWaveObject->m_pstrFrameName, "SlashWave", 12);
		slashWaveObject->GetMaterial(0)->SetTexture(pSlashWaveTexture);
	}
}

void ResourceManager::LoadGambler(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, unsigned int index)
{
	if (!GamblerModel[index]) {
		GamblerModel[index] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Gambler.bin", nullptr);
		
		Texture *pBodyTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBodyTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Body_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBodyTexture, 15, false);
		SkinnedFrameObject* Body = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Body");
		strncpy(Body->m_pstrFrameName, "Gambler_Body_Color", 19);
		Body->GetMaterial(0)->SetTexture(pBodyTexture);

		Texture *pFaceTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pFaceTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_face_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pFaceTexture, 15, false);
		SkinnedFrameObject* face = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Head");
		strncpy(face->m_pstrFrameName, "Gambler_face_Base_Color", 24);
		face->GetMaterial(0)->SetTexture(pFaceTexture);

		Texture *pHairTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHairTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_hair_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHairTexture, 15, false);
		SkinnedFrameObject* hair = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Hair");
		strncpy(hair->m_pstrFrameName, "Gambler_hair_Base_Color", 24);
		hair->GetMaterial(0)->SetTexture(pHairTexture);

		Texture *pHatTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHatTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_hat_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHatTexture, 15, false);
		SkinnedFrameObject* hat = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Hat");
		strncpy(hat->m_pstrFrameName, "Gambler_hat_Base_Color", 23);
		hat->GetMaterial(0)->SetTexture(pHatTexture);

		Texture *pMaskTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pMaskTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_mask_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pMaskTexture, 15, false);
		SkinnedFrameObject* Mask = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Mask");
		strncpy(Mask->m_pstrFrameName, "Gambler_mask_Base_Color", 24);
		Mask->GetMaterial(0)->SetTexture(pMaskTexture);

		Texture *pCardTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pCardTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gamble_Card_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pCardTexture, 15, false);
		SkinnedFrameObject* card = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Card001");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Card002");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Card003");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Card004");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Card005");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Card006");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Card007");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel[index]->m_pModelRootObject->FindFrame("Gamble_Card008");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);


		for (unsigned int i = 0; i < 3; ++i) {
			if (!CardModel[index * 3 + i]) {
				CardModel[index * 3 + i] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Card.bin", nullptr);
				BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pCardTexture, 15, false);
				SkinnedFrameObject* cardObject = CardModel[index * 3 + i]->m_pModelRootObject->FindFrame("Box001");
				strncpy(cardObject->m_pstrFrameName, "Card", 5);
				cardObject->GetMaterial(0)->SetTexture(pCardTexture);
			}
		}
	}
}

void ResourceManager::LoadElfArcher(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, unsigned int index)
{
	if (!ElfArcherModel[index]) {
		ElfArcherModel[index] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ElfArcher.bin", nullptr);
		
		Texture *pBodyTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBodyTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_Body_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBodyTexture, 15, false);
		SkinnedFrameObject* Body = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Body");
		strncpy(Body->m_pstrFrameName, "ElfArcher_Body_Base_Color", 26);
		Body->GetMaterial(0)->SetTexture(pBodyTexture);

		Texture *pHairTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHairTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_hair.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHairTexture, 15, false);
		SkinnedFrameObject* hair = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Hair");
		strncpy(hair->m_pstrFrameName, "ElfArcher_Hair_Color", 21);
		hair->GetMaterial(0)->SetTexture(pHairTexture);

		Texture *pBowTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBowTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_Bow_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBowTexture, 15, false);
		SkinnedFrameObject* bow = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Bow");
		strncpy(bow->m_pstrFrameName, "ElfArcher_Bow_Base_Color", 25);
		bow->GetMaterial(0)->SetTexture(pBowTexture);

		Texture *pBeltTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBeltTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_Belt.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBeltTexture, 15, false);
		SkinnedFrameObject* belt = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Belt");
		strncpy(belt->m_pstrFrameName, "ElfArcher_Belt_Color", 25);
		belt->GetMaterial(0)->SetTexture(pBeltTexture);

		Texture *pArrowBagTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pArrowBagTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_ArrowBag_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowBagTexture, 15, false);
		SkinnedFrameObject* arrowBag = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_ArrowBag");
		strncpy(arrowBag->m_pstrFrameName, "Elf_ArrowBag_Base_Color", 25);
		arrowBag->GetMaterial(0)->SetTexture(pArrowBagTexture);
		
		Texture *pArrowTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pArrowTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_Arrow_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowTexture, 15, false);
		SkinnedFrameObject* arrow = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow001");
		strncpy(arrow->m_pstrFrameName, "ElfArcher_Arrow_Base_Color", 27);
		arrow->GetMaterial(0)->SetTexture(pArrowTexture);

		arrow = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow002");
		strncpy(arrow->m_pstrFrameName, "ElfArcher_Arrow_Base_Color", 27);
		arrow->GetMaterial(0)->SetTexture(pArrowTexture);

		arrow = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow003");
		strncpy(arrow->m_pstrFrameName, "ElfArcher_Arrow_Base_Color", 27);
		arrow->GetMaterial(0)->SetTexture(pArrowTexture);

		arrow = ElfArcherModel[index]->m_pModelRootObject->FindFrame("Elf_Arrow004");
		strncpy(arrow->m_pstrFrameName, "ElfArcher_Arrow_Base_Color", 27);
		arrow->GetMaterial(0)->SetTexture(pArrowTexture);
		
		for (unsigned int i = 0; i < 7; ++i) {
			ArrowModel[index * 7 + i] = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Arrow.bin", nullptr);
			BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowTexture, 15, false);
			SkinnedFrameObject* arrowObject = ArrowModel[index * 7 + i]->m_pModelRootObject->FindFrame("Cylinder005");
			strncpy(arrowObject->m_pstrFrameName, "Arrow", 6);
			arrowObject->GetMaterial(0)->SetTexture(pArrowTexture);
		}
	}
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

SkinnedFrameObject * ResourceManager::GetArrowbject(unsigned int index) const
{
	return ArrowModel[index]->m_pModelRootObject;
}

SkinnedFrameObject * ResourceManager::GetPortalObject(unsigned int index) const
{
	return PortalModel[index]->m_pModelRootObject;
}