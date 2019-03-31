#pragma once
#include "Shader000_BaseShader.h"
class SkyBoxShader :
	public BaseShader
{
public:
	SkyBoxShader();
	virtual ~SkyBoxShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

