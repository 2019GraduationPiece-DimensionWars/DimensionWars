#include "stdafx.h"
#include "Shader005_TextureRectangleShader.h"


TextureRectangleShader::TextureRectangleShader()
{
}


TextureRectangleShader::~TextureRectangleShader()
{
}

D3D12_INPUT_LAYOUT_DESC TextureRectangleShader::CreateInputLayout()
{
	unsigned int nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE TextureRectangleShader::CreateVertexShader()
{
	return(BaseShader::CompileShaderFromFile(L"BasicShaders.hlsl", "VSTextured", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE TextureRectangleShader::CreatePixelShader()
{
	return(BaseShader::CompileShaderFromFile(L"BasicShaders.hlsl", "PSTextured", "ps_5_1", &m_pd3dPixelShaderBlob));
}
