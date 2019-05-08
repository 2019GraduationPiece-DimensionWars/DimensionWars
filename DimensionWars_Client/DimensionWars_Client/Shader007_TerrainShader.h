#pragma once
#include "Shader000_BaseShader.h"

class TerrainShader :
	public BaseShader
{
public:
	TerrainShader();
	virtual ~TerrainShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

