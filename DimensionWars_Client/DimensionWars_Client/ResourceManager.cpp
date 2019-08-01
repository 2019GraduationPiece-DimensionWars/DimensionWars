#include "stdafx.h"
#include "ResourceManager.h"
#include "AnimationController.h"
#include "Object002_SkinnedFrameObject.h"


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
	if (!GrimReaperModel) GrimReaperModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/GrimReaper.bin", nullptr);
	if (!GamblerModel) GamblerModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Gambler.bin", nullptr);
	if (!ElfArcherModel) ElfArcherModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ElfArcher.bin", nullptr);
	
	if (!SlashWaveModel) SlashWaveModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SlashWave.bin", nullptr);
	if (!CardModel) CardModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Card.bin", nullptr);
	if (!ArrowModel) ArrowModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Arrow.bin", nullptr);
	if (!PortalModel) PortalModel = SkinnedFrameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Portal.bin", nullptr);
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