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
	LoadGrimReaper(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	LoadGambler(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	LoadElfArcher(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (!PortalModel) {
		PortalModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Portal.bin", nullptr);
		Texture *pPortalTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pPortalTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Portal.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pPortalTexture, 15, false);
		SkinnedFrameObject* portalObject = PortalModel->m_pModelRootObject->FindFrame("Sphere001");
		strncpy(portalObject->m_pstrFrameName, "Portal", 8);
		portalObject->GetMaterial(0)->SetTexture(pPortalTexture);
	}
}

void ResourceManager::LoadGrimReaper(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	if (!GrimReaperModel) {
		GrimReaperModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/GrimReaper.bin", nullptr);
		
		Texture *pSkeletonTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pSkeletonTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/Death_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSkeletonTexture, 15, false);
		SkinnedFrameObject* skeleton = GrimReaperModel->m_pModelRootObject->FindFrame("Object001");
		strncpy(skeleton->m_pstrFrameName, "GrimReaper_Base_Color", 23);
		skeleton->GetMaterial(0)->SetTexture(pSkeletonTexture);
	
		Texture *pScytheTexture = new Texture(1, RESOURCE_TEXTURE2DARRAY, 0);
		pScytheTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/Death_Scythe_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pScytheTexture, 15, false);
		SkinnedFrameObject* scythe = GrimReaperModel->m_pModelRootObject->FindFrame("Cylinder002");
		strncpy(scythe->m_pstrFrameName, "GrimReaper_Scythe_Base_Color", 30);
		scythe->GetMaterial(0)->SetTexture(pScytheTexture);

		Texture *pCapeTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pCapeTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/Death_Cape_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pCapeTexture, 15, false);
		SkinnedFrameObject* cape = GrimReaperModel->m_pModelRootObject->FindFrame("Torus001");
		strncpy(cape->m_pstrFrameName, "GrimReaper_Cape_Base_Color", 27);
		cape->GetMaterial(0)->SetTexture(pCapeTexture);
	}

	if (!SlashWaveModel) {
		SlashWaveModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SlashWave.bin", nullptr);
		Texture *pSlashWaveTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pSlashWaveTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/GrimReaper/SlashWave_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSlashWaveTexture, 15, false);
		SkinnedFrameObject* slashWaveObject = SlashWaveModel->m_pModelRootObject->FindFrame("Box001");
		strncpy(slashWaveObject->m_pstrFrameName, "SlashWave", 12);
		slashWaveObject->GetMaterial(0)->SetTexture(pSlashWaveTexture);
	}
}

void ResourceManager::LoadGambler(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	if (!GamblerModel) {
		GamblerModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Gambler.bin", nullptr);
		
		Texture *pBodyTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBodyTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_Body_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBodyTexture, 15, false);
		SkinnedFrameObject* Body = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Body");
		strncpy(Body->m_pstrFrameName, "Gambler_Body_Color", 19);
		Body->GetMaterial(0)->SetTexture(pBodyTexture);

		Texture *pFaceTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pFaceTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_face_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pFaceTexture, 15, false);
		SkinnedFrameObject* face = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Head");
		strncpy(face->m_pstrFrameName, "Gambler_face_Base_Color", 24);
		face->GetMaterial(0)->SetTexture(pFaceTexture);

		Texture *pHairTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHairTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_hair_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHairTexture, 15, false);
		SkinnedFrameObject* hair = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Hair");
		strncpy(hair->m_pstrFrameName, "Gambler_hair_Base_Color", 24);
		hair->GetMaterial(0)->SetTexture(pHairTexture);

		Texture *pHatTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHatTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_hat_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHatTexture, 15, false);
		SkinnedFrameObject* hat = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Hat");
		strncpy(hat->m_pstrFrameName, "Gambler_hat_Base_Color", 23);
		hat->GetMaterial(0)->SetTexture(pHatTexture);

		Texture *pMaskTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pMaskTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gambler_mask_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pMaskTexture, 15, false);
		SkinnedFrameObject* Mask = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Mask");
		strncpy(Mask->m_pstrFrameName, "Gambler_mask_Base_Color", 24);
		Mask->GetMaterial(0)->SetTexture(pMaskTexture);

		Texture *pCardTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pCardTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gamble_Card_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pCardTexture, 15, false);
		SkinnedFrameObject* card = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Card001");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Card002");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Card003");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Card004");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Card005");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Card006");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Card007");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Gamble_Card008");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		if (!CardModel) {
			CardModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Card.bin", nullptr);
			BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pCardTexture, 15, false);
			SkinnedFrameObject* cardObject = CardModel->m_pModelRootObject->FindFrame("Box001");
			strncpy(cardObject->m_pstrFrameName, "Card", 5);
			cardObject->GetMaterial(0)->SetTexture(pCardTexture);
		}
	}
}

void ResourceManager::LoadElfArcher(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	if (!ElfArcherModel) {
		ElfArcherModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ElfArcher.bin", nullptr);
		
		Texture *pBodyTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBodyTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_Body_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBodyTexture, 15, false);
		SkinnedFrameObject* Body = ElfArcherModel->m_pModelRootObject->FindFrame("Elf_Body");
		strncpy(Body->m_pstrFrameName, "ElfArcher_Body_Base_Color", 26);
		Body->GetMaterial(0)->SetTexture(pBodyTexture);

		Texture *pHairTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHairTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_hair.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHairTexture, 15, false);
		SkinnedFrameObject* hair = ElfArcherModel->m_pModelRootObject->FindFrame("Elf_Hair");
		strncpy(hair->m_pstrFrameName, "ElfArcher_Hair_Color", 21);
		hair->GetMaterial(0)->SetTexture(pHairTexture);

		Texture *pBowTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBowTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_Bow_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBowTexture, 15, false);
		SkinnedFrameObject* bow = ElfArcherModel->m_pModelRootObject->FindFrame("Elf_Bow");
		strncpy(bow->m_pstrFrameName, "ElfArcher_Bow_Base_Color", 25);
		bow->GetMaterial(0)->SetTexture(pBowTexture);

		Texture *pBeltTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pBeltTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_Belt.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBeltTexture, 15, false);
		SkinnedFrameObject* belt = ElfArcherModel->m_pModelRootObject->FindFrame("Elf_Belt");
		strncpy(belt->m_pstrFrameName, "ElfArcher_Belt_Color", 25);
		belt->GetMaterial(0)->SetTexture(pBeltTexture);

		Texture *pArrowBagTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pArrowBagTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_ArrowBag_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowBagTexture, 15, false);
		SkinnedFrameObject* arrowBag = ElfArcherModel->m_pModelRootObject->FindFrame("Elf_ArrowBag");
		strncpy(arrowBag->m_pstrFrameName, "ElfArcher_Belt_Color", 25);
		arrowBag->GetMaterial(0)->SetTexture(pArrowBagTexture);
		
		Texture *pArrowTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pArrowTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/ElfArcher/Elf_Arrow_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowTexture, 15, false);
		SkinnedFrameObject* arrow = ElfArcherModel->m_pModelRootObject->FindFrame("Elf_Arrow001");
		strncpy(arrow->m_pstrFrameName, "ElfArcher_Arrow_Base_Color", 27);
		arrow->GetMaterial(0)->SetTexture(pArrowTexture);

		arrow = ElfArcherModel->m_pModelRootObject->FindFrame("Elf_Arrow002");
		strncpy(arrow->m_pstrFrameName, "ElfArcher_Arrow_Base_Color", 27);
		arrow->GetMaterial(0)->SetTexture(pArrowTexture);

		arrow = ElfArcherModel->m_pModelRootObject->FindFrame("Elf_Arrow003");
		strncpy(arrow->m_pstrFrameName, "ElfArcher_Arrow_Base_Color", 27);
		arrow->GetMaterial(0)->SetTexture(pArrowTexture);

		arrow = ElfArcherModel->m_pModelRootObject->FindFrame("Elf_Arrow004");
		strncpy(arrow->m_pstrFrameName, "ElfArcher_Arrow_Base_Color", 27);
		arrow->GetMaterial(0)->SetTexture(pArrowTexture);
		
		if (!ArrowModel) {
			ArrowModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Arrow.bin", nullptr);
			BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pArrowTexture, 15, false);
			SkinnedFrameObject* arrowObject = ArrowModel->m_pModelRootObject->FindFrame("Cylinder005");
			strncpy(arrowObject->m_pstrFrameName, "Arrow", 6);
			arrowObject->GetMaterial(0)->SetTexture(pArrowTexture);
		}
	}
}

SkinnedFrameObject * ResourceManager::GetSlashWaveObject() const
{
	return SlashWaveModel->m_pModelRootObject;
}

SkinnedFrameObject * ResourceManager::GetCardObject() const
{
	return CardModel->m_pModelRootObject;
}

SkinnedFrameObject * ResourceManager::GetArrowbject() const
{
	return ArrowModel->m_pModelRootObject;
}


SkinnedFrameObject * ResourceManager::GetPortalObject() const
{
	return PortalModel->m_pModelRootObject;
}