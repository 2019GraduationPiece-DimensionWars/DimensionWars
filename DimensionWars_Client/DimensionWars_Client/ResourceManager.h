#pragma once

class LoadedModelInfo;

class ResourceManager
{
private:
	LoadedModelInfo* GrimReaperModel = nullptr;
	LoadedModelInfo* GamblerModel = nullptr;
	LoadedModelInfo* ElfArcherModel = nullptr;

	LoadedModelInfo* SlashWaveModel = nullptr;
	LoadedModelInfo* CardModel = nullptr;
	LoadedModelInfo* ArrowModel = nullptr;

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
};

