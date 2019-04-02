#pragma once
#include "Shader000_BaseShader.h"
class StandardLightingShader :
	public BaseShader
{
public:
	StandardLightingShader();
	virtual ~StandardLightingShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

