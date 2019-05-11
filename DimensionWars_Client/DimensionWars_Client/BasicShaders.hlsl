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
struct VS_WIREFRAME_INPUT
{
    float3 position : POSITION;
};

struct VS_WIREFRAME_OUTPUT
{
    float4 position : SV_POSITION;
};

VS_WIREFRAME_OUTPUT VSWireFrame(VS_WIREFRAME_INPUT input)
{
    VS_WIREFRAME_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);

    return (output);
}

float4 PSWireFrame(VS_WIREFRAME_OUTPUT input) : SV_TARGET
{
    return (float4(0.75f, 0.1f, 0.1f, 1.0f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


struct VS_SKINNED_WIREFRAME_INPUT
{
    float3 position : POSITION;
    int4 indices : BONEINDEX;
    float4 weights : BONEWEIGHT;
};

struct VS_SKINNED_WIREFRAME_OUTPUT
{
    float4 position : SV_POSITION;
};

VS_SKINNED_WIREFRAME_OUTPUT VSSkinnedAnimationWireFrame(VS_SKINNED_WIREFRAME_INPUT input)
{
    VS_SKINNED_WIREFRAME_OUTPUT output;

    float3 positionW = float3(0.0f, 0.0f, 0.0f);
    matrix mtxVertexToBoneWorld;
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
    {
        mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
        positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
    }

    output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);

    return (output);
}

float4 PSSkinnedAnimationWireFrame(VS_SKINNED_WIREFRAME_OUTPUT input) : SV_TARGET
{
    return (float4(0.5f, 0.5f, 0.5f, 1.0f));
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


struct VS_STANDARD_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float3 bitangentW : BITANGENT;
    float2 uv : TEXCOORD;
};

VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

    output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
    output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
        cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_SPECULAR_MAP)
        cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_METALLIC_MAP)
        cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
    float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_EMISSION_MAP)
        cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);
  
    float3 normalW;
    float4 cColor = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
    {
        float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
        float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
        normalW = normalize(mul(vNormal, TBN));
    }
    else
    {
        normalW = normalize(input.normalW);
    }
    float4 cIllumination = Lighting(input.positionW, normalW);
    return (lerp(cColor, cIllumination, 0.5f));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////

struct VS_SKINNED_STANDARD_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    int4 indices : BONEINDEX;
    float4 weights : BONEWEIGHT;
};

VS_STANDARD_OUTPUT VSSkinnedAnimationStandard(VS_SKINNED_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

	//output.positionW = float3(0.0f, 0.0f, 0.0f);
	//output.normalW = float3(0.0f, 0.0f, 0.0f);
	//output.tangentW = float3(0.0f, 0.0f, 0.0f);
	//output.bitangentW = float3(0.0f, 0.0f, 0.0f);
	//matrix mtxVertexToBoneWorld;
	//for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	//{
	//	mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
	//	output.positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
	//	output.normalW += input.weights[i] * mul(input.normal, (float3x3)mtxVertexToBoneWorld);
	//	output.tangentW += input.weights[i] * mul(input.tangent, (float3x3)mtxVertexToBoneWorld);
	//	output.bitangentW += input.weights[i] * mul(input.bitangent, (float3x3)mtxVertexToBoneWorld);
	//}
    float4x4 mtxVertexToBoneWorld = (float4x4) 0.0f;
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
    {
		// mtxVertexToBoneWorld += input.weights[i] * gpmtxBoneTransforms[input.indices[i]];
        mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
    }
    output.positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
    output.normalW = mul(input.normal, (float3x3) mtxVertexToBoneWorld).xyz;
    output.tangentW = mul(input.tangent, (float3x3) mtxVertexToBoneWorld).xyz;
    output.bitangentW = mul(input.bitangent, (float3x3) mtxVertexToBoneWorld).xyz;

    output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;

    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


struct VS_TERRAIN_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
    VS_TERRAIN_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.color = input.color;
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;

    return (output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
    float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gssWrap, input.uv0);
    float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gssWrap, input.uv1);
//	float4 cColor = saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));
    float4 cColor = input.color * saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));

    return (cColor);
}

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
