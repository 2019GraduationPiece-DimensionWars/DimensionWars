#pragma once

class SkinnedFrameObject;
class SkinnedMesh;

struct CALLBACKKEY
{
	float  							m_fTime = 0.0f;
	void  							*m_pCallbackData = nullptr;
};

#define _WITH_ANIMATION_INTERPOLATION

class AnimationCallbackHandler
{
public:
	AnimationCallbackHandler() { }
	~AnimationCallbackHandler() { }

public:
	virtual void HandleCallback(void *pCallbackData) { }
};

class AnimationCurve
{
public:
	AnimationCurve(int nKeys);
	~AnimationCurve();

public:
	int								m_nKeys = 0;

	float							*m_pfKeyTimes = nullptr;
	float							*m_pfKeyValues = nullptr;

public:
	float GetValueByLinearInterpolation(float fPosition);
};

class AnimationLayer
{
public:
	AnimationLayer();
	~AnimationLayer();

public:
	float							m_fWeight = 1.0f;

	int								m_nAnimatedBoneFrames = 0;
	SkinnedFrameObject				**m_ppAnimatedBoneFrameCaches = NULL; //[m_nAnimatedBoneFrames]

	AnimationCurve					*(*m_ppAnimationCurves)[9] = NULL;

public:
	void LoadAnimationKeyValues(int nBoneFrame, int nCurve, FILE *pInFile);

	XMFLOAT4X4 GetSRT(int nBoneFrame, float fPosition);
};

class AnimationSet
{
public:
	AnimationSet(float fStartTime, float fEndTime, char *pstrName);
	~AnimationSet();

public:
	char							m_pstrAnimationSetName[64];

	int								m_nAnimationLayers = 0;
	AnimationLayer					*m_pAnimationLayers = nullptr;

	float							m_fStartTime = 0.0f;
	float							m_fEndTime = 0.0f;
	float							m_fLength = 0.0f;

	float 							m_fPosition = 0.0f;
	int 							m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong

	int 							m_nCallbackKeys = 0;
	CALLBACKKEY 					*m_pCallbackKeys = nullptr;

	AnimationCallbackHandler 		*m_pAnimationCallbackHandler = nullptr;

public:
	void SetPosition(float fTrackPosition);

	void Animate(float fTrackPosition, float fTrackWeight);

	void SetCallbackKeys(int nCallbackKeys);
	void SetCallbackKey(int nKeyIndex, float fTime, void *pData);
	void SetAnimationCallbackHandler(AnimationCallbackHandler *pCallbackHandler);

	void *GetCallbackData();
};

class AnimationSets
{
private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	AnimationSets(int nAnimationSets);
	~AnimationSets();

public:
	int								m_nAnimationSets = 0;
	AnimationSet					**m_ppAnimationSets = nullptr;

public:
	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void *pData);
	void SetAnimationCallbackHandler(int nAnimationSet, AnimationCallbackHandler *pCallbackHandler);
};

class CAnimationTrack
{
public:
	CAnimationTrack() { }
	~CAnimationTrack() { }

public:
	BOOL 							m_bEnable = true;
	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	float 							m_fWeight = 1.0f;

	int 							m_nAnimationSet = 0;

public:
	void SetAnimationSet(int nAnimationSet) { m_nAnimationSet = nAnimationSet; }

	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetWeight(float fWeight) { m_fWeight = fWeight; }
	void SetPosition(float fPosition) { m_fPosition = fPosition; }
};

class LoadedModelInfo
{
public:
	LoadedModelInfo() { }
	~LoadedModelInfo();

public:
	SkinnedFrameObject				*m_pModelRootObject = nullptr;

	AnimationSets					*m_pAnimationSets = nullptr;

	int 							m_nSkinnedMeshes = 0;
	SkinnedMesh						**m_ppSkinnedMeshes = nullptr; //[SkinnedMeshes], Skinned Mesh Cache

public:
	void PrepareSkinning();
};

class AnimationController
{
public:
	AnimationController(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nAnimationTracks, LoadedModelInfo *pModel);
	~AnimationController();

public:
	float 							m_fTime = 0.0f;

	int 							m_nAnimationTracks = 0;
	CAnimationTrack 				*m_pAnimationTracks = NULL;

	AnimationSets					*m_pAnimationSets = NULL;

	int 							m_nSkinnedMeshes = 0;
	SkinnedMesh						**m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

	ID3D12Resource					**m_ppd3dcbSkinningBoneTransforms = NULL; //[SkinnedMeshes]
	XMFLOAT4X4						**m_ppcbxmf4x4MappedSkinningBoneTransforms = NULL;

public:
	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackEnable(int nAnimationTrack, bool bEnable);
	void SetTrackPosition(int nAnimationTrack, float fPosition);
	void SetTrackSpeed(int nAnimationTrack, float fSpeed);
	void SetTrackWeight(int nAnimationTrack, float fWeight);

	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void *pData);
	void SetAnimationCallbackHandler(int nAnimationSet, AnimationCallbackHandler *pCallbackHandler);

	void AdvanceTime(float fElapsedTime, SkinnedFrameObject *pRootGameObject);
};
