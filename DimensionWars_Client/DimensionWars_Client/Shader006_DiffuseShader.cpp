#include "stdafx.h"
#include "Shader006_DiffuseShader.h"


DiffuseShader::DiffuseShader()
{
}


DiffuseShader::~DiffuseShader()
{
}

D3D12_INPUT_LAYOUT_DESC DiffuseShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE DiffuseShader::CreateVertexShader()
{
	return(BaseShader::CompileShaderFromFile(L"BasicShaders.hlsl", "VSDiffused", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE DiffuseShader::CreatePixelShader()
{
	return(BaseShader::CompileShaderFromFile(L"BasicShaders.hlsl", "PSDiffused", "ps_5_1", &m_pd3dPixelShaderBlob));
}
