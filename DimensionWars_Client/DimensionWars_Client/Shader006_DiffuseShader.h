#pragma once
#include "Shader000_BaseShader.h"
class DiffuseShader :
	public BaseShader
{
public:
	DiffuseShader();
	virtual ~DiffuseShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

