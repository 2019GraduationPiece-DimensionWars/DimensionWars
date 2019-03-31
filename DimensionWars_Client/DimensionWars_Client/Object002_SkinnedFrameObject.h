#pragma once
#include "Object000_BaseObject.h"
#include "Material.h"

class BaseShader;
class SkinnedMesh;
class LoadedModelInfo;
class AnimationController;


class SkinnedFrameObject :
	public BaseObject
{
private:
	int								m_nReferences = 0;

	

public:
	void AddRef();
	virtual void Release();


public:
	XMFLOAT4X4						m_xmf4x4ToParent;

	char							m_pstrFrameName[64];

	
	SkinnedFrameObject 					*m_pParent = nullptr;
	SkinnedFrameObject 					*m_pChild = nullptr;
	SkinnedFrameObject 					*m_pSibling = nullptr;

	SkinnedFrameObject();
	SkinnedFrameObject(unsigned int nMaterials, unsigned int nMeshes = 1);
	virtual ~SkinnedFrameObject();	

	void SetWireFrameShader();
	void SetSkinnedAnimationWireFrameShader();

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(XMFLOAT3 xmf3Position);

	void SetScale(XMFLOAT3 xmf3Scale = { 1.0f, 1.0f, 1.0f });
	void SetScale(float x, float y, float z);

	void SetChild(SkinnedFrameObject *pChild, bool bReferenceUpdate = false);
	SkinnedFrameObject *GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent = nullptr);
	SkinnedFrameObject *FindFrame(char *pstrFrameName);

	virtual void Rotate(float fPitch, float fYaw, float fRoll) override;
	virtual void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle) override final;
	virtual void Rotate(XMFLOAT4 *pxmf4Quaternion) override final;

	//

	AnimationController 			*m_pSkinnedAnimationController = nullptr;

	SkinnedMesh *FindSkinnedMesh(char *pstrSkinnedMeshName);
	void FindAndSetSkinnedMesh(SkinnedMesh **ppSkinnedMeshes, int *pnSkinnedMesh);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackAnimationPosition(int nAnimationTrack, float fPosition);

	static void LoadAnimationFromFile(FILE *pInFile, LoadedModelInfo *pLoadedModel);
	static SkinnedFrameObject *LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, SkinnedFrameObject *pParent, FILE *pInFile, BaseShader *pShader, int *pnSkinnedMeshes);

	static LoadedModelInfo *LoadGeometryAndAnimationFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName, BaseShader *pShader, bool flag3DsMaxCoordinates = true);

	static void PrintFrameInfo(SkinnedFrameObject *pGameObject, SkinnedFrameObject *pParent);



	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, BaseCamera *pCamera = nullptr);
};

