#pragma once

class LoadedModelInfo;
class SkinnedFrameObject;

class ResourceManager
{
private:
	LoadedModelInfo* GrimReaperModel[MAX_PLAYER] = { nullptr };
	LoadedModelInfo* GamblerModel[MAX_PLAYER] = { nullptr };
	LoadedModelInfo* ElfArcherModel[MAX_PLAYER] = { nullptr };
	
	LoadedModelInfo* SlashWaveModel[MAX_PLAYER] = { nullptr };
	LoadedModelInfo* CardModel[MAX_PLAYER * 3] = { nullptr };
	LoadedModelInfo* ArrowModel[MAX_PLAYER * 7] = { nullptr };
	LoadedModelInfo* PortalModel[Potal_end - Potal_start] = { nullptr };

public:
	ResourceManager();
	virtual ~ResourceManager();

	void AllModelLoad(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	void LoadGrimReaper(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, unsigned int index = 0);
	void LoadGambler(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, unsigned int index = 0);
	void LoadElfArcher(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, unsigned int index = 0);

	LoadedModelInfo* GetGrimReaperModel(unsigned int index = 0) const { if (GrimReaperModel[index]) return GrimReaperModel[index]; return nullptr; }
	LoadedModelInfo* GetGamblerModel(unsigned int index = 0) const { if (GamblerModel[index]) return GamblerModel[index]; return nullptr; }
	LoadedModelInfo* GetElfArcherModel(unsigned int index = 0) const { if (ElfArcherModel[index]) return ElfArcherModel[index]; return nullptr; }

	LoadedModelInfo* GetSlashWaveModel(unsigned int index = 0) const { if (SlashWaveModel[index]) return SlashWaveModel[index]; return nullptr; }
	LoadedModelInfo* GetCardModel(unsigned int index = 0) const { if (CardModel[index]) return CardModel[index]; return nullptr; }
	LoadedModelInfo* GetArrowModel(unsigned int index = 0) const { if (ArrowModel[index]) return ArrowModel[index]; return nullptr; }
	LoadedModelInfo* GetPortalModel(unsigned int index = 0) const { if (PortalModel[index]) return PortalModel[index]; return nullptr; }


	SkinnedFrameObject* GetSlashWaveObject(unsigned int index = 0) const;
	SkinnedFrameObject* GetCardObject(unsigned int index = 0) const;
	SkinnedFrameObject* GetArrowbject(unsigned int index = 0) const;
	SkinnedFrameObject* GetPortalObject(unsigned int index = 0) const;
};

