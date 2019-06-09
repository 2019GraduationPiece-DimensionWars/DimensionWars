#include "stdafx.h"
#include "Mesh000_BaseMesh.h"
#include "Material.h"
#include "Object000_BaseObject.h"
#include "Shader000_BaseShader.h"

/*
BaseObject::BaseObject(unsigned int nMeshes)
{
	m_xmf4x4World = Matrix4x4::Identity();

	m_nMeshes = nMeshes;
	m_ppMeshes = nullptr;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new BaseMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = nullptr;
	}
}*/

BaseObject::BaseObject()
{
	m_xmf4x4World = Matrix4x4::Identity();
}

BaseObject::BaseObject(unsigned int nMaterials) : BaseObject()
{
	/*m_nMeshes = nMeshes;
	m_ppMeshes = new BaseMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	
		m_ppMeshes[i] = nullptr;*/

	m_nMaterials = nMaterials;
	m_ppMaterials = nullptr;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new Material*[m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}


BaseObject::~BaseObject()
{
	// ReleaseShaderVariables();

	if (m_pMesh) m_pMesh->Release();

	if (m_nMaterials > 0) {
		for (int i = 0; i < m_nMaterials; i++)
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
	}
	if (m_ppMaterials)
		delete[] m_ppMaterials;

	/*
	if (m_ppMeshes) {
		for (int i = 0; i < m_nMeshes; i++) {
			if (m_ppMeshes[i]) 
				m_ppMeshes[i]->Release();
			m_ppMeshes[i] = nullptr;
		}
		delete[] m_ppMeshes;
	}
*/
}

void BaseObject::SetMesh(BaseMesh * pMesh, int nIndex)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
	/*
	if (m_ppMeshes) {
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}*/
}

void BaseObject::SetShader(BaseShader * pShader)
{
	m_nMaterials = 1;
	m_ppMaterials = new Material*[m_nMaterials];
	m_ppMaterials[0] = new Material(0);
	m_ppMaterials[0]->SetShader(pShader);
}

void BaseObject::SetShader(int nMaterial, BaseShader * pShader)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->SetShader(pShader);
}

void BaseObject::BuildShaders(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void BaseObject::SetMaterial(int nMaterial, Material * pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

void BaseObject::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void **)&m_pcbMappedGameObject);
}

void BaseObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject) {
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
	// if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}

void BaseObject::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
}

void BaseObject::UpdateShaderVariable(ID3D12GraphicsCommandList * pd3dCommandList, XMFLOAT4X4 * pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void BaseObject::UpdateShaderVariable(ID3D12GraphicsCommandList * pd3dCommandList, Material * pMaterial)
{
}

void BaseObject::Animate(float fTimeElapsed)
{
}

void BaseObject::OnPrepareRender(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
}

void BaseObject::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void BaseObject::OnPrepareRender()
{
}

void BaseObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	OnPrepareRender(pd3dCommandList, pCamera);
/*
	if (m_nMaterials > 0) {
		for (int i = 0; i < m_nMaterials; i++) {
			if (m_ppMaterials[i]) {
				if (m_ppMaterials[i]->m_pShader)
					m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
				m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
			}
		}
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);

	if (m_ppMeshes) {
		for (int i = 0; i < m_nMeshes; i++) {
			if (m_ppMeshes[i])
				m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}*/
	
	if (m_pMesh) {
		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0) {
			for (int i = 0; i < m_nMaterials; i++) {
				if (m_ppMaterials[i]) {
					if (m_ppMaterials[i]->m_pShader) 
						m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
					m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
				}
				
				m_pMesh->Render(pd3dCommandList, i);
			}
		}
	}
}

void BaseObject::BuildLight()
{
}

void BaseObject::BuildMaterial()
{
}

void BaseObject::BuildLightsAndMaterials()
{
}

void BaseObject::ReleaseUploadBuffers()
{

	for (int i = 0; i < m_nMaterials; i++)
		if (m_ppMaterials[i]) 
			m_ppMaterials[i]->ReleaseUploadBuffers();
}

XMFLOAT3 BaseObject::GetPosition() const
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 BaseObject::GetLook() const
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 BaseObject::GetUp() const
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 BaseObject::GetRight() const
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void BaseObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void BaseObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void BaseObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	BaseObject::SetPosition(xmf3Position);
}

void BaseObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	BaseObject::SetPosition(xmf3Position);
}

void BaseObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	BaseObject::SetPosition(xmf3Position);
}

void BaseObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void BaseObject::Rotate(XMFLOAT3 * pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void BaseObject::Rotate(XMFLOAT4 * pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

unsigned int BaseObject::GetMeshType(int nIndex)
{
	/*
	if (m_nMeshes >= nIndex)
		return((m_ppMeshes[nIndex]) ? m_ppMeshes[nIndex]->GetType() : 0x00);	
		*/
	return((m_pMesh) ? m_pMesh->GetType() : 0x00);
}

void BaseObject::SetLookAt(XMFLOAT3 & xmf3Target, XMFLOAT3 & xmf3Up)
{
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(xmf3Position, xmf3Target, xmf3Up);
	m_xmf4x4World._11 = mtxLookAt._11; m_xmf4x4World._12 = mtxLookAt._21; m_xmf4x4World._13 = mtxLookAt._31;
	m_xmf4x4World._21 = mtxLookAt._12; m_xmf4x4World._22 = mtxLookAt._22; m_xmf4x4World._23 = mtxLookAt._32;
	m_xmf4x4World._31 = mtxLookAt._13; m_xmf4x4World._32 = mtxLookAt._23; m_xmf4x4World._33 = mtxLookAt._33;
	/*
		XMFLOAT3 xmf3Look = Vector3::Normalize(Vector3::Subtract(xmf3Target, xmf3Position));
		XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
		xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);
		m_xmf4x4World._11 = xmf3Right.x; m_xmf4x4World._12 = xmf3Right.y; m_xmf4x4World._13 = xmf3Right.z;
		m_xmf4x4World._21 = xmf3Up.x; m_xmf4x4World._22 = xmf3Up.y; m_xmf4x4World._23 = xmf3Up.z;
		m_xmf4x4World._31 = xmf3Look.x; m_xmf4x4World._32 = xmf3Look.y; m_xmf4x4World._33 = xmf3Look.z;
		*/
}
