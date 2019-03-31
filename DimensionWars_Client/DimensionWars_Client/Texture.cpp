#include "stdafx.h"
#include "Texture.h"


Texture::Texture(int nTextureResources, unsigned int nTextureType, int nSamplers)
{
	m_nTextureType = nTextureType;
	m_nTextures = nTextureResources;

	if (m_nTextures > 0) {
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppTextureUploadBuffers = new ID3D12Resource*[m_nTextures];
		m_ppTextures = new ID3D12Resource*[m_nTextures];
	}

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) 
		m_pSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

Texture::~Texture()
{
	if (m_ppTextures)	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppTextures[i]) m_ppTextures[i]->Release();
		delete[] m_ppTextures;
	}

	if (m_pRootArgumentInfos)
		delete[] m_pRootArgumentInfos;

	if (m_pSamplerGpuDescriptorHandles) 
		delete[] m_pSamplerGpuDescriptorHandles;
}

void Texture::SetRootArgument(int nIndex, unsigned int nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void Texture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void Texture::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (m_nTextureType == RESOURCE_TEXTURE2D_ARRAY)
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	else 
		for (int i = 0; i < m_nTextures; i++)
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
}

void Texture::UpdateShaderVariable(ID3D12GraphicsCommandList * pd3dCommandList, int nIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle);
}

void Texture::ReleaseShaderVariables()
{
}

void Texture::LoadTextureFromFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, wchar_t * pszFileName, unsigned int nIndex, bool bIsDDSFile)
{
	if (bIsDDSFile)
		m_ppTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &(m_ppTextureUploadBuffers[nIndex]), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	else
		m_ppTextures[nIndex] = ::CreateTextureResourceFromWICFile(pd3dDevice, pd3dCommandList, pszFileName, &(m_ppTextureUploadBuffers[nIndex]), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Texture::ReleaseUploadBuffers()
{
	if (m_ppTextureUploadBuffers) {
		for (int i = 0; i < m_nTextures; i++) 
			if (m_ppTextureUploadBuffers[i]) 
				m_ppTextureUploadBuffers[i]->Release();
		delete[] m_ppTextureUploadBuffers;
		m_ppTextureUploadBuffers = NULL;
	}
}
