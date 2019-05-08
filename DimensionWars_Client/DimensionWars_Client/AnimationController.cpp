#include "stdafx.h"
#include "AnimationController.h"
#include "Object002_SkinnedFrameObject.h"
#include "Mesh003_SkinnedMesh.h"


AnimationCurve::AnimationCurve(int nKeys)
{
	m_nKeys = nKeys;
	m_pfKeyTimes = new float[nKeys];
	m_pfKeyValues = new float[nKeys];
}

AnimationCurve::~AnimationCurve()
{
	if (m_pfKeyTimes) delete[] m_pfKeyTimes;
	if (m_pfKeyValues) delete[] m_pfKeyValues;
}

float AnimationCurve::GetValueByLinearInterpolation(float fPosition)
{
	for (int k = 0; k < (m_nKeys - 1); ++k)
	{
		if ((m_pfKeyTimes[k] <= fPosition) && (fPosition <= m_pfKeyTimes[k + 1]))
		{
			float t = (fPosition - m_pfKeyTimes[k]) / (m_pfKeyTimes[k + 1] - m_pfKeyTimes[k]);
			return(m_pfKeyValues[k] * (1.0f - t) + m_pfKeyValues[k + 1] * t);
		}
	}
	return(m_pfKeyValues[m_nKeys - 1]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AnimationLayer::AnimationLayer()
{
}

AnimationLayer::~AnimationLayer()
{
	for (int i = 0; i < m_nAnimatedBoneFrames; ++i) {
		for (int j = 0; j < 9; ++j) {
			if (m_ppAnimationCurves[i][j]) 
				delete m_ppAnimationCurves[i][j];
		}
	}
	if (m_ppAnimationCurves) delete[] m_ppAnimationCurves;

	if (m_ppAnimatedBoneFrameCaches) delete[] m_ppAnimatedBoneFrameCaches;
}

void AnimationLayer::LoadAnimationKeyValues(int nBoneFrame, int nCurve, FILE * pInFile)
{
	int nAnimationKeys = ::ReadIntegerFromFile(pInFile);

	m_ppAnimationCurves[nBoneFrame][nCurve] = new AnimationCurve(nAnimationKeys);

	::fread(m_ppAnimationCurves[nBoneFrame][nCurve]->m_pfKeyTimes, sizeof(float), nAnimationKeys, pInFile);
	::fread(m_ppAnimationCurves[nBoneFrame][nCurve]->m_pfKeyValues, sizeof(float), nAnimationKeys, pInFile);
}

XMFLOAT4X4 AnimationLayer::GetSRT(int nBoneFrame, float fPosition)
{
	XMFLOAT4X4 xmf4x4Transform;
	XMFLOAT3 xmf3S = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 xmf3R = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 xmf3T = XMFLOAT3(0.0f, 0.0f, 0.0f);

	if (m_ppAnimationCurves[nBoneFrame][0]) xmf3T.x = m_ppAnimationCurves[nBoneFrame][0]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][1]) xmf3T.y = m_ppAnimationCurves[nBoneFrame][1]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][2]) xmf3T.z = m_ppAnimationCurves[nBoneFrame][2]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][3]) xmf3R.x = m_ppAnimationCurves[nBoneFrame][3]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][4]) xmf3R.y = m_ppAnimationCurves[nBoneFrame][4]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][5]) xmf3R.z = m_ppAnimationCurves[nBoneFrame][5]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][6]) xmf3S.x = m_ppAnimationCurves[nBoneFrame][6]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][7]) xmf3S.y = m_ppAnimationCurves[nBoneFrame][7]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][8]) xmf3S.z = m_ppAnimationCurves[nBoneFrame][8]->GetValueByLinearInterpolation(fPosition);

	XMMATRIX R = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationX(xmf3R.x), XMMatrixRotationY(xmf3R.y)), XMMatrixRotationZ(xmf3R.z));
	XMMATRIX S = XMMatrixScaling(xmf3S.x, xmf3S.y, xmf3S.z);
	XMMATRIX T = XMMatrixTranslation(xmf3T.x, xmf3T.y, xmf3T.z);
	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixMultiply(XMMatrixMultiply(S, R), T));

	return(xmf4x4Transform);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AnimationSet::AnimationSet(float fStartTime, float fEndTime, char * pstrName, int nType)
{
	m_fStartTime = fStartTime;
	m_fEndTime = fEndTime;
	m_fLength = fEndTime - fStartTime;
	m_nType = nType;

	strcpy_s(m_pstrAnimationSetName, 64, pstrName);
}

AnimationSet::~AnimationSet()
{
	// if (m_pAnimationLayers) delete[] m_pAnimationLayers;

	// if (m_pCallbackKeys) delete[] m_pCallbackKeys;
	if (m_pAnimationCallbackHandler) delete m_pAnimationCallbackHandler;
}

void AnimationSet::SetPosition(float fTrackPosition)
{
	m_fPosition = fTrackPosition;
	float fNowPosition = (m_fLength == 0.0f)? 0.0f:(::fmod(fTrackPosition, m_fLength));	// ::fmod(fTrackPosition, m_fLength)는 현재 진행된 애니메이션의 시간.  == fTrackPosition % m_fLength
	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		m_fPosition = m_fStartTime + fNowPosition;
		break;
	}
	case ANIMATION_TYPE_ONCE: 
	{
		if (!m_bEndTrigger) m_fPosition = m_fStartTime + fNowPosition;
		m_bEndTrigger = (m_fPosition >= m_fEndTime) ? true : false;
		printf("%s : TrackPos : %.2f / Length : %.2f / m_fPosition : %.2f / (%.2f ~ %.2f) / %.2f\n", m_pstrAnimationSetName, fTrackPosition, m_fLength, m_fPosition, m_fStartTime, m_fEndTime, fNowPosition);
		break;
	}
	case ANIMATION_TYPE_PINGPONG:
	{
		if (m_bEndTrigger) {
			m_fPosition = m_fEndTime - fNowPosition;
			if (m_fPosition <= m_fStartTime)
				m_bEndTrigger = false;
		}
		else {
			m_fPosition = m_fStartTime + fNowPosition;
			if (m_fPosition >= m_fEndTime)
				m_bEndTrigger = true;
		}
		break;
	}
	}
		
	if (m_pAnimationCallbackHandler)
	{
		void *pCallbackData = GetCallbackData();
		if (pCallbackData) m_pAnimationCallbackHandler->HandleCallback(pCallbackData);
	}
}

void AnimationSet::Animate(float fTrackPosition, float fTrackWeight)
{
	SetPosition(fTrackPosition);

	for (int i = 0; i < m_nAnimationLayers; i++)
		for (int j = 0; j < m_pAnimationLayers[i].m_nAnimatedBoneFrames; j++)
			m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = Matrix4x4::Zero();

	for (int i = 0; i < m_nAnimationLayers; i++)
		for (int j = 0; j < m_pAnimationLayers[i].m_nAnimatedBoneFrames; j++) {
			XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Scale(m_pAnimationLayers[i].GetSRT(j, m_fPosition), m_pAnimationLayers[i].m_fWeight * fTrackWeight);
			m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = Matrix4x4::Add(xmf4x4Transform, m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent);
		}
}

void AnimationSet::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void AnimationSet::SetCallbackKey(int nKeyIndex, float fKeyTime, void * pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void AnimationSet::SetAnimationCallbackHandler(AnimationCallbackHandler * pCallbackHandler)
{
	m_pAnimationCallbackHandler = pCallbackHandler;
}

void * AnimationSet::GetCallbackData()
{
	for (int i = 0; i < m_nCallbackKeys; i++) {
		if (::IsEqual(m_pCallbackKeys[i].m_fTime, m_fPosition, ANIMATION_CALLBACK_EPSILON))
			return(m_pCallbackKeys[i].m_pCallbackData);
	}

	return(nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AnimationSets::AnimationSets(int nAnimationSets)
{
	m_nAnimationSets = nAnimationSets;
	m_ppInitalAnimationSets = new AnimationSet*[m_nAnimationSets];

	std::vector<AnimationSet*> temp(&m_ppInitalAnimationSets[0],&m_ppInitalAnimationSets[m_nAnimationSets]);
	m_AnimationSets = temp;
}

AnimationSets::~AnimationSets()
{
	//if (m_ppInitalAnimationSets[0] && m_ppInitalAnimationSets[0]->m_pAnimationLayers) delete[] m_ppInitalAnimationSets[0]->m_pAnimationLayers;
	for (int i = 0; i < m_nAnimationSets; i++) if (m_ppInitalAnimationSets[i]) delete m_ppInitalAnimationSets[i];
	
	if (m_ppInitalAnimationSets) delete[] m_ppInitalAnimationSets;
	for (auto & data : m_AnimationSets) if (data) delete data;
}

void AnimationSets::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	m_ppInitalAnimationSets[nAnimationSet]->m_nCallbackKeys = nCallbackKeys;
	m_ppInitalAnimationSets[nAnimationSet]->m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void AnimationSets::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void * pData)
{
	m_ppInitalAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_ppInitalAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void AnimationSets::SetAnimationCallbackHandler(int nAnimationSet, AnimationCallbackHandler * pCallbackHandler)
{
	m_ppInitalAnimationSets[nAnimationSet]->SetAnimationCallbackHandler(pCallbackHandler);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LoadedModelInfo::~LoadedModelInfo()
{
	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
}

void LoadedModelInfo::PrepareSkinning()
{
	int nSkinnedMesh = 0;
	m_ppSkinnedMeshes = new SkinnedMesh*[m_nSkinnedMeshes];
	m_pModelRootObject->FindAndSetSkinnedMesh(m_ppSkinnedMeshes, &nSkinnedMesh);

	for (int i = 0; i < m_nSkinnedMeshes; i++) 
		m_ppSkinnedMeshes[i]->PrepareSkinning(m_pModelRootObject);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AnimationController::AnimationController(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nAnimationTracks, LoadedModelInfo * pModel)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks = new AnimationTrack[nAnimationTracks];
	
	m_pAnimationSets = pModel->m_pAnimationSets;
	m_pAnimationSets->AddRef();

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;
	m_ppSkinnedMeshes = new SkinnedMesh*[m_nSkinnedMeshes];
	for (int i = 0; i < m_nSkinnedMeshes; ++i) m_ppSkinnedMeshes[i] = pModel->m_ppSkinnedMeshes[i];

	m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource*[m_nSkinnedMeshes];
	m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4*[m_nSkinnedMeshes];

	unsigned int ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < m_nSkinnedMeshes; ++i) {
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, nullptr, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, nullptr, (void **)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}
}

AnimationController::~AnimationController()
{
	if (m_pAnimationTracks) delete[] m_pAnimationTracks;

	if (m_pAnimationSets) m_pAnimationSets->Release();

	for (int i = 0; i < m_nSkinnedMeshes; ++i) {
		m_ppd3dcbSkinningBoneTransforms[i]->Unmap(0, nullptr);
		m_ppd3dcbSkinningBoneTransforms[i]->Release();
	}
	if (m_ppd3dcbSkinningBoneTransforms) delete[] m_ppd3dcbSkinningBoneTransforms;
	if (m_ppcbxmf4x4MappedSkinningBoneTransforms) delete[] m_ppcbxmf4x4MappedSkinningBoneTransforms;

	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
}

void AnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; ++i) {
		m_ppSkinnedMeshes[i]->m_pd3dcbSkinningBoneTransforms = m_ppd3dcbSkinningBoneTransforms[i];
		m_ppSkinnedMeshes[i]->m_pcbxmf4x4MappedSkinningBoneTransforms = m_ppcbxmf4x4MappedSkinningBoneTransforms[i];
	}
}

void AnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetAnimationSet(nAnimationSet);
}

void AnimationController::SetTrackEnable(int nAnimationTrack, bool bEnable)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetEnable(bEnable);
}

void AnimationController::SetTrackPosition(int nAnimationTrack, float fPosition)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetPosition(fPosition);
}

void AnimationController::SetTrackSpeed(int nAnimationTrack, float fSpeed)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetSpeed(fSpeed);
}

void AnimationController::SetTrackWeight(int nAnimationTrack, float fWeight)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetWeight(fWeight);
}

void AnimationController::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackKeys(nAnimationSet, nCallbackKeys);
}

void AnimationController::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void * pData)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackKey(nAnimationSet, nKeyIndex, fKeyTime, pData);
}

void AnimationController::SetAnimationCallbackHandler(int nAnimationSet, AnimationCallbackHandler * pCallbackHandler)
{
	if (m_pAnimationSets) m_pAnimationSets->SetAnimationCallbackHandler(nAnimationSet, pCallbackHandler);
}

void AnimationController::SetAnimationSet(int nAnimationSet)
{
	if (m_pAnimationSets && (nAnimationSet < m_pAnimationSets->GetNumberOfAnimationSets()))
	{
	//	m_nAnimationSet = nAnimationSet;
		m_pAnimationTracks[m_nAnimationTrack].SetAnimationSet(nAnimationSet);
	}
}

void AnimationController::AdvanceTime(float fElapsedTime, SkinnedFrameObject * pRootGameObject)
{
	m_fTime += fElapsedTime;
	
	if (m_pAnimationTracks) {
		for (int i = 0; i < m_nAnimationTracks; ++i) m_pAnimationTracks[i].SetPosition(m_pAnimationTracks[i].GetPosition() + (fElapsedTime * m_pAnimationTracks[i].GetSpeed()));

		for (int i = 0; i < m_nAnimationTracks; ++i){
			if (m_pAnimationTracks[i].isEnable()) {
				AnimationSet *pAnimationSet = m_pAnimationSets->GetAnimationSet(m_pAnimationTracks[i].GetAnimationSet());
				pAnimationSet->Animate(m_pAnimationTracks[i].GetPosition(), m_pAnimationTracks[i].GetWeight());
			}
		}

		pRootGameObject->UpdateTransform(nullptr);
	}
}
