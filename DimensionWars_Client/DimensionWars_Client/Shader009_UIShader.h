#pragma once
#include "Shader005_TextureRectangleShader.h"
class UIShader :
	public TextureRectangleShader
{
public:
	UIShader();
	virtual ~UIShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};

