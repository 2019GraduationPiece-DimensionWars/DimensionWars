#pragma once

class LoadedModelInfo;
class SkinnedFrameObject;

class ResourceManager
{
private:
	LoadedModelInfo* GrimReaperModel = nullptr;
	LoadedModelInfo* GamblerModel = nullptr;
	LoadedModelInfo* ElfArcherModel = nullptr;

	LoadedModelInfo* SlashWaveModel = nullptr;
	LoadedModelInfo* CardModel = nullptr;
	LoadedModelInfo* ArrowModel = nullptr;
	LoadedModelInfo* PortalModel = nullptr;

public:
	ResourceManager();
	virtual ~ResourceManager();

	void AllModelLoad(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	LoadedModelInfo* GetGrimReaperModel() const { return GrimReaperModel; }
	LoadedModelInfo* GetGamblerModel() const { return GamblerModel; }
	LoadedModelInfo* GetElfArcherModel() const { return ElfArcherModel; }

	LoadedModelInfo* GetSlashWaveModel() const { return SlashWaveModel; }
	LoadedModelInfo* GetCardModel() const { return CardModel; }
	LoadedModelInfo* GetArrowModel() const { return ArrowModel; }
	LoadedModelInfo* GetPortalModel() const { return PortalModel; }


	SkinnedFrameObject* GetSlashWaveObject() const;
	SkinnedFrameObject* GetCardObject() const;
	SkinnedFrameObject* GetArrowbject() const;
	SkinnedFrameObject* GetPortalObject() const;
};

