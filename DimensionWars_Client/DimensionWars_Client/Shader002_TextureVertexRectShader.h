#pragma once
#include "Shader000_BaseShader.h"
class TextureVertexRectShader :
	public BaseShader
{
public:
	TextureVertexRectShader();
	virtual ~TextureVertexRectShader();


	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader();
};

