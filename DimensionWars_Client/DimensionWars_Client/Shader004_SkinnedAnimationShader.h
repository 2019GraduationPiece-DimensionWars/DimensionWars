#pragma once
#include "Shader003_StandardLightingShader.h"
class SkinnedAnimationShader :
	public StandardLightingShader
{
public:
	SkinnedAnimationShader();
	virtual ~SkinnedAnimationShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
};

