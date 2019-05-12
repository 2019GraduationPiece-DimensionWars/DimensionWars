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
	SkinnedFrameObject				**m_ppAnimatedBoneFrameCaches = nullptr; //[m_nAnimatedBoneFrames]

	AnimationCurve					*(*m_ppAnimationCurves)[9] = nullptr;	// S, R, T 벡터 각각의 x,y,z 원소

public:
	void LoadAnimationKeyValues(int nBoneFrame, int nCurve, FILE *pInFile);

	XMFLOAT4X4 GetSRT(int nBoneFrame, float fPosition);
};

class AnimationSet
{
public:
	AnimationSet(float fStartTime, float fEndTime, char *pstrName, int nType = ANIMATION_TYPE_LOOP);
	~AnimationSet();

public:
	char							m_pstrAnimationSetName[64];

	int								m_nAnimationLayers = 0;
	AnimationLayer					*m_pAnimationLayers = nullptr;	// 유니크포인터로 바꿔주는 공사가 필요하다.

	float							m_fStartTime = 0.0f;
	float							m_fEndTime = 0.0f;
	float							m_fLength = 0.0f;

	float							m_fOldPosition = 0.0f;
	float 							m_fPosition = 0.0f;
	int 							m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong

	bool							m_bStartTrigger = true; // 이 애니메이션이 시작되었을 때.
	bool							m_bEndTrigger = false;	// Once, PingPong일 때 이 애니메이션이 끝났는지를 검사하기 위함
	bool							m_bPingPongTrigger = false;

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

	void SetEndOff() { m_bEndTrigger = false; }
	void SetEndOn() { m_bEndTrigger = true; }
	const bool isEnd() const { return m_bEndTrigger; }
};

class AnimationSets
{
private:
	int								m_nReferences = 0;
	int								m_nAnimationSets = 0;
	AnimationSet					**m_ppInitalAnimationSets = nullptr;

	std::vector<AnimationSet*>		m_AnimationSets;

public:
	void AddRef() { ++m_nReferences; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	AnimationSets(int nAnimationSets);
	~AnimationSets();
	
	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void *pData);
	void SetAnimationCallbackHandler(int nAnimationSet, AnimationCallbackHandler *pCallbackHandler);

	int GetNumberOfAnimationSets() const { return m_nAnimationSets; }
	void AddAnimationSet(AnimationSet* other) {
		++m_nAnimationSets;
		m_AnimationSets.emplace_back(other); 
		m_AnimationSets[m_nAnimationSets - 1]->m_nAnimationLayers = m_AnimationSets[m_nAnimationSets - 2]->m_nAnimationLayers;
		m_AnimationSets[m_nAnimationSets - 1]->m_pAnimationLayers = m_AnimationSets[m_nAnimationSets - 2]->m_pAnimationLayers;
	}
	void SetAnimationSet(int nAnimationSet, AnimationSet* other) { m_AnimationSets[nAnimationSet] = other; }
	AnimationSet* GetAnimationSet(int nAnimationSet) const { return m_AnimationSets[nAnimationSet]; }
	void AddAnimationSet(float fStartTime, float fEndTime, char *pstrName, int nType = ANIMATION_TYPE_LOOP) { 
		++m_nAnimationSets;
		m_AnimationSets.emplace_back(new AnimationSet(fStartTime, fEndTime, pstrName, nType)); 
		m_AnimationSets[m_nAnimationSets - 1]->m_nAnimationLayers = m_AnimationSets[m_nAnimationSets - 2]->m_nAnimationLayers;
		m_AnimationSets[m_nAnimationSets - 1]->m_pAnimationLayers = m_AnimationSets[m_nAnimationSets - 2]->m_pAnimationLayers;
	}
};

class AnimationTrack
{
public:
	AnimationTrack() { }
	~AnimationTrack() { }

private:
	BOOL 							m_bEnable = true;
	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	float 							m_fWeight = 1.0f;

	int 							m_nAnimationSet = 0;

public:
	void SetAnimationSet(int nAnimationSet) { m_nAnimationSet = nAnimationSet; }
	int GetAnimationSet() const { return m_nAnimationSet; }

	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
	bool isEnable() const { return m_bEnable; }

	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	float GetSpeed() const { return m_fSpeed; }

	void SetWeight(float fWeight) { m_fWeight = fWeight; }
	float GetWeight() const { return m_fWeight; }

	void SetPosition(float fPosition) { m_fPosition = fPosition; }
	float GetPosition() const { return m_fPosition; }
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

	int								m_nAnimationTrack = 0;	// 현재 재생중인 트랙
	int 							m_nAnimationTracks = 0; // 총 트랙 갯수
	AnimationTrack 					*m_pAnimationTracks = nullptr;

	AnimationSets					*m_pAnimationSets = nullptr;	// 애니메이션셋 들을 관리하는 클래스

	int 							m_nSkinnedMeshes = 0;
	SkinnedMesh						**m_ppSkinnedMeshes = nullptr; //[SkinnedMeshes], Skinned Mesh Cache

	ID3D12Resource					**m_ppd3dcbSkinningBoneTransforms = nullptr; //[SkinnedMeshes]
	XMFLOAT4X4						**m_ppcbxmf4x4MappedSkinningBoneTransforms = nullptr;

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

	void SetAnimationSet(int nAnimationSet);

	void AddAnimationSet(float fStartTime, float fEndTime, char *pstrName, int nType = ANIMATION_TYPE_LOOP) { if (m_pAnimationSets) m_pAnimationSets->AddAnimationSet(fStartTime, fEndTime, pstrName, nType); }

	void AdvanceTime(float fElapsedTime, SkinnedFrameObject *pRootGameObject);
};
