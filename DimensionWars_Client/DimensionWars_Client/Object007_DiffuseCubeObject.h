#pragma once
#include "Object000_BaseObject.h"
class DiffuseCubeObject :
	public BaseObject
{
public:
	DiffuseCubeObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float size = 1500.0f);
	virtual ~DiffuseCubeObject();

	void Build(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float size = 1500.0f);
};


class TextureCubeObject :
	public BaseObject
{
public:
	TextureCubeObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float size = 1500.0f);
	virtual ~TextureCubeObject();

	void Build(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float size = 1500.0f);
};

class TexturePortalObject :
	public BaseObject
{
public:
	TexturePortalObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float size = 1500.0f);
	virtual ~TexturePortalObject();

	void Build(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float size = 1500.0f);
};