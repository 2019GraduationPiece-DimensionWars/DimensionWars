struct MATERIAL
{
    float4 m_cAmbient;
    float4 m_cDiffuse;
    float4 m_cSpecular; //a = power
    float4 m_cEmissive;
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
    MATERIAL gMaterial : packoffset(c4);
    uint gnTexturesMask : packoffset(c8);
};

////
#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			128

cbuffer cbBoneOffsets : register(b7)
{
    float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b8)
{
    float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};
////


Texture2D gtxtTerrainBaseTexture : register(t1);
Texture2D gtxtTerrainDetailTexture : register(t2);

Texture2DArray TextureArray : register(t4);


#include "LightShader.hlsl"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define _WITH_VERTEX_LIGHTING

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

Texture2D gtxtTexture : register(t14);
TextureCube CubeTexture : register(t15);

Texture2D gtxtAlbedoTexture : register(t6);
Texture2D gtxtSpecularTexture : register(t7);
Texture2D gtxtNormalTexture : register(t8);
Texture2D gtxtMetallicTexture : register(t9);
Texture2D gtxtEmissionTexture : register(t10);
Texture2D gtxtDetailAlbedoTexture : register(t11);
Texture2D gtxtDetailNormalTexture : register(t12);

//

TextureCube gtxtSkyCubeTexture : register(t13);

SamplerState gssWrap : register(s0);
SamplerState gssClamp : register(s1);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


struct VS_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_CUBEMAP_OUTPUT
{
	float3 positionL : POSITION;
	float4 position : SV_POSITION;
};

VS_CUBEMAP_OUTPUT VSCubeMap(VS_CUBEMAP_INPUT input)
{
	VS_CUBEMAP_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.positionL = input.position;

	return (output);
}

float4 PSCubeMap(VS_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = CubeTexture.Sample(gssClamp, input.positionL);

	return (cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

//정점 셰이더의 입력을 위한 구조체를 선언한다. 
struct VS_DIFFUSED_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다. 
struct VS_DIFFUSED_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSDiffused(VS_DIFFUSED_INPUT input)
{
    VS_DIFFUSED_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.color = input.color;

    return (output);
}

float4 PSDiffused(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
    return (input.color);
}
