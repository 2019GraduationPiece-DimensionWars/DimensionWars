#pragma once
#include "Shader000_BaseShader.h"
class CubeShader :
	public BaseShader
{
public:
	CubeShader();
	virtual ~CubeShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	//virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

