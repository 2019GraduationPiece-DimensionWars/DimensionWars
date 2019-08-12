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
struct VS_SKYBOX_CUBEMAP_INPUT
{
    float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
    float3 positionL : POSITION;
    float4 position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
    VS_SKYBOX_CUBEMAP_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.positionL = input.position;

    return (output);
}

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtSkyCubeTexture.Sample(gssClamp, input.positionL);

    return (cColor);
}


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



///////////////



struct VS_TEXTURED_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

#ifdef _WITH_CONSTANT_BUFFER_SYNTAX
	output.position = mul(mul(mul(float4(input.position, 1.0f), gcbGameObjectInfo.mtxWorld), gcbCameraInfo.mtxView), gcbCameraInfo.mtxProjection);
#else
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
#endif
    output.uv = input.uv;

    return (output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input, uint primitiveID : SV_PrimitiveID) : SV_TARGET
{
    float4 cColor = gtxtTexture.Sample(gssWrap, input.uv);

    return (cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTUREVERTEX_INPUT
{
    float3 position : POSITION;
    float2 size : SIZE;
};

struct VS_TEXTUREVERTEX_OUTPUT
{
    float3 center : POSITION;
    float2 size : SIZE;
};

struct GS_TEXTUREVERTEX_OUTPUT
{
    float4 positionH : SV_POSITION;
    float3 positionWorld : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

VS_TEXTUREVERTEX_OUTPUT VSTextureVertex(VS_TEXTUREVERTEX_INPUT input)
{
    VS_TEXTUREVERTEX_OUTPUT output;

    output.center = input.position;
    output.size = input.size;

    return (output);
}

[maxvertexcount(4)]
void GSTextureVertex(point VS_TEXTUREVERTEX_OUTPUT input[1], uint primID : SV_PrimitiveID,inout TriangleStream<GS_TEXTUREVERTEX_OUTPUT> outStream)
{
    float3 vLook  = float3(0.0f, 0.0f, -1.0f);
    float3 vUp;
    vLook = normalize(vLook);
    float3 vRight = cross(vUp, vLook);
    float halfWidth = input[0].size.x * 0.5f;
    float halfHeight = input[0].size.y * 0.5f;
    float4 pVertice[4];
    pVertice[0] = float4(input[0].center + halfWidth * vRight - halfHeight * vUp, 1.0f);
    pVertice[1] = float4(input[0].center + halfWidth * vRight + halfHeight * vUp, 1.0f);
    pVertice[2] = float4(input[0].center - halfWidth * vRight - halfHeight * vUp, 1.0f);
    pVertice[3] = float4(input[0].center - halfWidth * vRight + halfHeight * vUp, 1.0f);

    float2 pUVs[4] = { float2(0.0f, 1.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(1.0f, 0.0f) };

    GS_TEXTUREVERTEX_OUTPUT gsOut;
    for (int i = 0; i < 4; ++i)
    {
        gsOut.positionWorld = pVertice[i].xyz;
        gsOut.positionH = mul(mul(pVertice[i], gmtxView), gmtxProjection);
        gsOut.normal = vLook;
        gsOut.uv = pUVs[i];
        gsOut.primID = primID;
        outStream.Append(gsOut);
    }

}

float4 PSTextureVertex(GS_TEXTUREVERTEX_OUTPUT input) : SV_TARGET
{
    float3 uvw = float3(input.uv, (input.primID % 4));
    float4 cTexture = TextureArray.Sample(gssWrap, uvw);
    float4 cColor = cTexture;
    return (cColor);
}