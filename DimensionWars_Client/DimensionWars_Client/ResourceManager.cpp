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
		pBodyTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gamble_body_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pBodyTexture, 15, false);
		SkinnedFrameObject* Body = GamblerModel->m_pModelRootObject->FindFrame("Object006");
		strncpy(Body->m_pstrFrameName, "Gambler_body_Base_Color", 24);
		Body->GetMaterial(0)->SetTexture(pBodyTexture);

		Texture *pFaceTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pFaceTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gamble_face_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pFaceTexture, 15, false);
		SkinnedFrameObject* face = GamblerModel->m_pModelRootObject->FindFrame("Object002");
		strncpy(face->m_pstrFrameName, "Gambler_face_Base_Color", 24);
		face->GetMaterial(0)->SetTexture(pFaceTexture);

		//Texture *pHairTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		//pHairTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gamble_hair_Base_Color.dds", 0);
		//BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHairTexture, 15, false);
		//SkinnedFrameObject* hair = GamblerModel->m_pModelRootObject->FindFrame("Object002");
		//strncpy(hair->m_pstrFrameName, "Gambler_hair_Base_Color", 24);
		//hair->GetMaterial(0)->SetTexture(pHairTexture);

		Texture *pHatTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pHatTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gamble_hat_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pHatTexture, 15, false);
		SkinnedFrameObject* hat = GamblerModel->m_pModelRootObject->FindFrame("Object005");
		strncpy(hat->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		hat->GetMaterial(0)->SetTexture(pHatTexture);

		Texture *pCardTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
		pCardTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Model/Gambler/Gamble_Card_Base_Color.dds", 0);
		BaseScene::CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pCardTexture, 15, false);
		SkinnedFrameObject* card = GamblerModel->m_pModelRootObject->FindFrame("Box001");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Box002");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Box003");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Box004");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Box005");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Box006");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Box007");
		strncpy(card->m_pstrFrameName, "Gambler_Card_Base_Color", 24);
		card->GetMaterial(0)->SetTexture(pCardTexture);

		card = GamblerModel->m_pModelRootObject->FindFrame("Box008");
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
		
		if (!ArrowModel) {
			ArrowModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Arrow.bin", nullptr);
			
		
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