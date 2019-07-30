#include "stdafx.h"
#include "Shader008_CubeShader.h"


CubeShader::CubeShader()
{
}


CubeShader::~CubeShader()
{
}

D3D12_INPUT_LAYOUT_DESC CubeShader::CreateInputLayout()
{
	unsigned int nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}



D3D12_SHADER_BYTECODE CubeShader::CreateVertexShader()
{
	return(BaseShader::CompileShaderFromFile(L"BasicShaders.hlsl", "VSCubeMap", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CubeShader::CreatePixelShader()
{
	return(BaseShader::CompileShaderFromFile(L"BasicShaders.hlsl", "PSCubeMap", "ps_5_1", &m_pd3dPixelShaderBlob));
}

