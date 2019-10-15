#pragma once

class LoadedModelInfo;
class SkinnedFrameObject;

class ResourceManager
{
private:
	LoadedModelInfo* GrimReaperModel[MAX_PLAYER] = { nullptr };
	LoadedModelInfo* GamblerModel[MAX_PLAYER] = { nullptr };
	LoadedModelInfo* ElfArcherModel[MAX_PLAYER] = { nullptr };
	
	LoadedModelInfo* SlashWaveModel[Slash_end - Slash_start] = { nullptr };
	LoadedModelInfo* CardModel[Card_end - Card_start] = { nullptr };
	LoadedModelInfo* ArrowModel[Arrow_end - Arrow_start] = { nullptr };
	LoadedModelInfo* PortalModel[Potal_end - Potal_start] = { nullptr };

public:
	ResourceManager();
	virtual ~ResourceManager();

	void AllModelLoad(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	void LoadGrimReaper(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, unsigned int index = 0);
	void LoadGambler(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, unsigned int index = 0);
	void LoadElfArcher(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, unsigned int index = 0);

	void LoadSlashWave(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	void LoadArrow(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	void LoadCard(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	LoadedModelInfo* GetGrimReaperModel(unsigned int index = 0) const;
	LoadedModelInfo* GetGamblerModel(unsigned int index = 0) const;
	LoadedModelInfo* GetElfArcherModel(unsigned int index = 0) const;

	LoadedModelInfo* GetSlashWaveModel(unsigned int index = 0) const;
	LoadedModelInfo* GetCardModel(unsigned int index = 0) const;
	LoadedModelInfo* GetArrowModel(unsigned int index = 0) const;
	LoadedModelInfo* GetPortalModel(unsigned int index = 0) const;

	SkinnedFrameObject* GetSlashWaveObject(unsigned int index = 0) const;
	SkinnedFrameObject* GetCardObject(unsigned int index = 0) const;
	SkinnedFrameObject* GetArrowObject(unsigned int index = 0) const;
	SkinnedFrameObject* GetPortalObject(unsigned int index = 0) const;
};

