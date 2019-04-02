#include "stdafx.h"
#include "AnimationController.h"
#include "Object002_SkinnedFrameObject.h"
#include "Mesh002_HierarchyMesh.h"
#include "Mesh003_SkinnedMesh.h"
#include "Material.h"
#include "Shader000_BaseShader.h"


void SkinnedFrameObject::AddRef()
{
	++m_nReferences;

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void SkinnedFrameObject::Release()
{
	if (m_pChild) m_pChild->Release();
	if (m_pSibling) m_pSibling->Release();

	if (--m_nReferences <= 0) delete this;
}

SkinnedFrameObject::SkinnedFrameObject() 
{
	m_xmf4x4ToParent = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
	Rotate(-90.0f, 0.0f, 0.0f);
}

SkinnedFrameObject::SkinnedFrameObject(unsigned int nMaterials, unsigned int nMeshes) : SkinnedFrameObject()
{
	m_nMaterials = nMaterials;
	if (m_nMaterials > 0) {
		m_ppMaterials = new Material*[m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = nullptr;
	}

	// nMeshes를 쓰는 코드가 아니다.
	/*
	m_nMeshes = 1;
	if (m_nMeshes > 0) {
		m_ppMeshes = new BaseMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = nullptr;
	}*/
}


SkinnedFrameObject::~SkinnedFrameObject()
{
	/*
	if (m_nMeshes > 0) {
		for (int i = 0; i < m_nMeshes; i++) {
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
		}
	}
	if (m_ppMeshes) delete[] m_ppMeshes;
*/
	if (m_pSkinnedAnimationController) delete m_pSkinnedAnimationController;
}

void SkinnedFrameObject::SetWireFrameShader()
{
	m_nMaterials = 1;
	m_ppMaterials = new Material*[m_nMaterials];
	m_ppMaterials[0] = nullptr;
	Material *pMaterial = new Material(0);
	pMaterial->SetWireFrameShader();
	SetMaterial(0, pMaterial);
}

void SkinnedFrameObject::SetSkinnedAnimationWireFrameShader()
{
	m_nMaterials = 1;
	m_ppMaterials = new Material*[m_nMaterials];
	m_ppMaterials[0] = nullptr;
	Material *pMaterial = new Material(0);
	pMaterial->SetSkinnedAnimationWireFrameShader();
	SetMaterial(0, pMaterial);
}

void SkinnedFrameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParent._41 = x;
	m_xmf4x4ToParent._42 = y;
	m_xmf4x4ToParent._43 = z;
	
	UpdateTransform(nullptr);
}

void SkinnedFrameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SkinnedFrameObject::SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void SkinnedFrameObject::SetScale(XMFLOAT3 xmf3Scale)
{
	XMMATRIX mtxScale = XMMatrixScaling(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(nullptr);
}

void SkinnedFrameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(nullptr);
}

void SkinnedFrameObject::SetChild(SkinnedFrameObject * pChild, bool bReferenceUpdate)
{
	if (pChild) {
		pChild->m_pParent = this;
		if (bReferenceUpdate)
			pChild->AddRef();
	}
	if (m_pChild) {
		if (pChild) 
			pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else {
		m_pChild = pChild;
	}
}

void SkinnedFrameObject::UpdateTransform(XMFLOAT4X4 * pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

SkinnedFrameObject * SkinnedFrameObject::FindFrame(char * pstrFrameName)
{
	SkinnedFrameObject *pFrameObject = nullptr;
	if (!strcmp(m_pstrFrameName, pstrFrameName)) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return nullptr;
}

void SkinnedFrameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(nullptr);
}

void SkinnedFrameObject::Rotate(XMFLOAT3 * pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(nullptr);
}

void SkinnedFrameObject::Rotate(XMFLOAT4 * pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(nullptr);
}

SkinnedMesh * SkinnedFrameObject::FindSkinnedMesh(char * pstrSkinnedMeshName)
{
	SkinnedMesh *pSkinnedMesh = NULL;
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT)) {
		pSkinnedMesh = (SkinnedMesh *)m_pMesh;
		if (!strcmp(pSkinnedMesh->m_pstrMeshName, pstrSkinnedMeshName)) return(pSkinnedMesh);
	}

	if (m_pSibling) if (pSkinnedMesh = m_pSibling->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);
	if (m_pChild) if (pSkinnedMesh = m_pChild->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);

	return nullptr;
}

void SkinnedFrameObject::FindAndSetSkinnedMesh(SkinnedMesh ** ppSkinnedMeshes, int * pnSkinnedMesh)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT)) ppSkinnedMeshes[(*pnSkinnedMesh)++] = (SkinnedMesh *)m_pMesh;

	if (m_pSibling) m_pSibling->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
	if (m_pChild) m_pChild->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
}

void SkinnedFrameObject::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackAnimationSet(nAnimationTrack, nAnimationSet);
}

void SkinnedFrameObject::SetTrackAnimationPosition(int nAnimationTrack, float fPosition)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackPosition(nAnimationTrack, fPosition);
}

void SkinnedFrameObject::LoadAnimationFromFile(FILE * pInFile, LoadedModelInfo * pLoadedModel)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nAnimationSets = 0;

	while (true) {
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<AnimationSets>:")) {
			nAnimationSets = ::ReadIntegerFromFile(pInFile);
			pLoadedModel->m_pAnimationSets = new AnimationSets(nAnimationSets);
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:")) {
			int nAnimationSet = ::ReadIntegerFromFile(pInFile);

			::ReadStringFromFile(pInFile, pstrToken); //Animation Set Name

			float fStartTime = ::ReadFloatFromFile(pInFile);
			float fEndTime = ::ReadFloatFromFile(pInFile);

			pLoadedModel->m_pAnimationSets->m_ppAnimationSets[nAnimationSet] = new AnimationSet(fStartTime, fEndTime, pstrToken);
			AnimationSet *pAnimationSet = pLoadedModel->m_pAnimationSets->m_ppAnimationSets[nAnimationSet];

			::ReadStringFromFile(pInFile, pstrToken);
			if (!strcmp(pstrToken, "<AnimationLayers>:")) {
				pAnimationSet->m_nAnimationLayers = ::ReadIntegerFromFile(pInFile);
				pAnimationSet->m_pAnimationLayers = new AnimationLayer[pAnimationSet->m_nAnimationLayers];

				for (int i = 0; i < pAnimationSet->m_nAnimationLayers; i++) {
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<AnimationLayer>:")) {
						int nAnimationLayer = ::ReadIntegerFromFile(pInFile);
						AnimationLayer *pAnimationLayer = &pAnimationSet->m_pAnimationLayers[nAnimationLayer];

						pAnimationLayer->m_nAnimatedBoneFrames = ::ReadIntegerFromFile(pInFile);

						pAnimationLayer->m_ppAnimatedBoneFrameCaches = new SkinnedFrameObject *[pAnimationLayer->m_nAnimatedBoneFrames];
						pAnimationLayer->m_ppAnimationCurves = new AnimationCurve *[pAnimationLayer->m_nAnimatedBoneFrames][9];

						pAnimationLayer->m_fWeight = ::ReadFloatFromFile(pInFile);

						for (int j = 0; j < pAnimationLayer->m_nAnimatedBoneFrames; j++) {
							::ReadStringFromFile(pInFile, pstrToken);
							if (!strcmp(pstrToken, "<AnimationCurve>:")) {
								int nCurveNode = ::ReadIntegerFromFile(pInFile); //j

								for (int k = 0; k < 9; k++) pAnimationLayer->m_ppAnimationCurves[j][k] = NULL;

								::ReadStringFromFile(pInFile, pstrToken);
								pAnimationLayer->m_ppAnimatedBoneFrameCaches[j] = pLoadedModel->m_pModelRootObject->FindFrame(pstrToken);

								while (true) {
									::ReadStringFromFile(pInFile, pstrToken);
									if (!strcmp(pstrToken, "<TX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 0, pInFile);
									else if (!strcmp(pstrToken, "<TY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 1, pInFile);
									else if (!strcmp(pstrToken, "<TZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 2, pInFile);
									else if (!strcmp(pstrToken, "<RX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 3, pInFile);
									else if (!strcmp(pstrToken, "<RY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 4, pInFile);
									else if (!strcmp(pstrToken, "<RZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 5, pInFile);
									else if (!strcmp(pstrToken, "<SX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 6, pInFile);
									else if (!strcmp(pstrToken, "<SY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 7, pInFile);
									else if (!strcmp(pstrToken, "<SZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 8, pInFile);
									else if (!strcmp(pstrToken, "</AnimationCurve>")) {
										break;
									}
								}
							}
						}
						::ReadStringFromFile(pInFile, pstrToken); //</AnimationLayer>
					}
				}
				::ReadStringFromFile(pInFile, pstrToken); //</AnimationLayers>
			}
			::ReadStringFromFile(pInFile, pstrToken); //</AnimationSet>
		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
			break;
	}
}

SkinnedFrameObject * SkinnedFrameObject::LoadFrameHierarchyFromFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, SkinnedFrameObject * pParent, FILE * pInFile, BaseShader * pShader, int * pnSkinnedMeshes)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = ::ReadIntegerFromFile(pInFile);

	SkinnedFrameObject *pFrameObject = new SkinnedFrameObject();
	::ReadStringFromFile(pInFile, pFrameObject->m_pstrFrameName);

	while (true) {
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Transform>:")) {
			nReads = (UINT)::fread(&pFrameObject->m_xmf4x4ToParent, sizeof(XMFLOAT4X4), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:")) {
			HierarchyMesh *pMesh = new HierarchyMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pFrameObject->SetMesh(pMesh);
			
			/**/pFrameObject->SetWireFrameShader();
		}
		else if (!strcmp(pstrToken, "<SkinDeformations>:")) {
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;

			SkinnedMesh *pSkinnedMesh = new SkinnedMesh(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->LoadSkinDeformationsFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pSkinnedMesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

			::ReadStringFromFile(pInFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			pFrameObject->SetMesh(pSkinnedMesh);

			/**/pFrameObject->SetSkinnedAnimationWireFrameShader();
		}
		else if (!strcmp(pstrToken, "<Children>:")) {
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0) {
				for (int i = 0; i < nChilds; i++) {
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<Frame>:")) {
						SkinnedFrameObject *pChild = SkinnedFrameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFrameObject, pInFile, pShader, pnSkinnedMeshes);
						if (pChild) pFrameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
						TCHAR pstrDebug[256] = { 0 };
						_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
						OutputDebugString(pstrDebug);
#endif
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
			break;
	}
	return(pFrameObject);
}

LoadedModelInfo * SkinnedFrameObject::LoadGeometryAndAnimationFromFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, char * pstrFileName, BaseShader * pShader, bool flag3DsMaxCoordinates)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	LoadedModelInfo *pLoadedModel = new LoadedModelInfo();
	pLoadedModel->m_pModelRootObject = new SkinnedFrameObject();
	strcpy_s(pLoadedModel->m_pModelRootObject->m_pstrFrameName, "RootNode");

	char pstrToken[64] = { '\0' };

	while (true) {
		if (::ReadStringFromFile(pInFile, pstrToken)) {
			if (!strcmp(pstrToken, "<Hierarchy>")) {
				while (true) {
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<Frame>:")) {
						SkinnedFrameObject *pChild = SkinnedFrameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader, &pLoadedModel->m_nSkinnedMeshes);
						if (pChild) 
							pLoadedModel->m_pModelRootObject->SetChild(pChild);
					}
					else if (!strcmp(pstrToken, "</Hierarchy>"))
						break;
				}
			}
			else if (!strcmp(pstrToken, "<Animation>")) {
				SkinnedFrameObject::LoadAnimationFromFile(pInFile, pLoadedModel);
				pLoadedModel->PrepareSkinning();
			}
			else if (!strcmp(pstrToken, "</Animation>"))
				break;
		}
		else
			break;
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pLoadedModel->m_pModelRootObject, NULL);
#endif

	::fclose(pInFile);
	if (flag3DsMaxCoordinates)
		pLoadedModel->m_pModelRootObject->Rotate(90.0f, 0.0f, 0.0f);	// 3D Max의 좌표계를 강제로 변환하기 위한 회전
	return(pLoadedModel);
}

void SkinnedFrameObject::PrintFrameInfo(SkinnedFrameObject * pGameObject, SkinnedFrameObject * pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) SkinnedFrameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) SkinnedFrameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

void SkinnedFrameObject::Animate(float fTimeElapsed)
{
	OnPrepareRender();

	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}

void SkinnedFrameObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, BaseCamera * pCamera)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pMesh)
	{
		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
					m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
				}

				m_pMesh->Render(pd3dCommandList, i);
			}
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}
