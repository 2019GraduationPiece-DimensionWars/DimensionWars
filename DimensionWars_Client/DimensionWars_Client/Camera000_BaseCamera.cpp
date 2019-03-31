#include "stdafx.h"
#include "Camera000_BaseCamera.h"


BaseCamera::BaseCamera()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_Viewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_ScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fTimeLag = 0.0f;
	m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_nMode = 0x00;

	m_nMode = FIRST_PERSON_CAMERA;
}

BaseCamera::BaseCamera(BaseCamera *pCamera)
{
	if (pCamera)
		*this = *pCamera;
	else {
		m_xmf4x4View = Matrix4x4::Identity();
		m_xmf4x4Projection = Matrix4x4::Identity();
		m_Viewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
		m_ScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
		m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = 0.0f;
		m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_fTimeLag = 0.0f;
		m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_nMode = 0x00;
	}
}

BaseCamera::~BaseCamera()
{
}

void BaseCamera::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	unsigned int ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pCBCamera = ::CreateBufferResource(pd3dDevice, pd3dCommandList, nullptr, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

	m_pCBCamera->Map(0, NULL, (void **)&m_pCBMappedCamera);
}

void BaseCamera::ReleaseShaderVariables()
{
	if (m_pCBCamera){
		m_pCBCamera->Unmap(0, NULL);
		m_pCBCamera->Release();
	}
}

void BaseCamera::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	::memcpy(&m_pCBMappedCamera->m_xmf4x4View, &xmf4x4View, sizeof(XMFLOAT4X4));

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	::memcpy(&m_pCBMappedCamera->m_xmf4x4Projection, &xmf4x4Projection, sizeof(XMFLOAT4X4));

	::memcpy(&m_pCBMappedCamera->m_xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pCBCamera->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
}

void BaseCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}

void BaseCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}

void BaseCamera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
}

void BaseCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	//XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	//::XMStoreFloat4x4(&m_xmf4x4Projection, xmmtxProjection);
}

void BaseCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_Viewport.TopLeftX = float(xTopLeft);
	m_Viewport.TopLeftY = float(yTopLeft);
	m_Viewport.Width = float(nWidth);
	m_Viewport.Height = float(nHeight);
	m_Viewport.MinDepth = fMinZ;
	m_Viewport.MaxDepth = fMaxZ;
}

void BaseCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_ScissorRect.left = xLeft;
	m_ScissorRect.top = yTop;
	m_ScissorRect.right = xRight;
	m_ScissorRect.bottom = yBottom;
}

void BaseCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_Viewport);
	pd3dCommandList->RSSetScissorRects(1, &m_ScissorRect);
}
