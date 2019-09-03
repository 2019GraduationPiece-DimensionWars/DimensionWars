#include "stdafx.h"
#include "Shader009_UIShader.h"


UIShader::UIShader()
{
}


UIShader::~UIShader()
{
}

D3D12_SHADER_BYTECODE UIShader::CreateVertexShader()
{
	return(BaseShader::CompileShaderFromFile(L"2DShaders.hlsl", "VSUITexture", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE UIShader::CreatePixelShader()
{
	return(BaseShader::CompileShaderFromFile(L"2DShaders.hlsl", "PSTextured", "ps_5_1", &m_pd3dPixelShaderBlob));
}
