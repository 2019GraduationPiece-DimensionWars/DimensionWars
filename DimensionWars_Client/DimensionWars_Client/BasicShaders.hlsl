cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
};

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
    return (float4(0.0f, 0.0f, 1.0f, 1.0f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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
    return (float4(1.0f, 1.0f, 0.0f, 1.0f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTerrainBaseTexture : register(t1);
Texture2D gtxtTerrainDetailTexture : register(t2);

SamplerState gssWrap : register(s0);

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

TextureCube gtxtSkyCubeTexture : register(t13);
SamplerState gssClamp : register(s1);

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtSkyCubeTexture.Sample(gssClamp, input.positionL);

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

Texture2DArray TextureArray: register(t4);
SamplerState gSamplerState : register(s0);

[maxvertexcount(4)]
void GSTextureVertex(point VS_TEXTUREVERTEX_OUTPUT input[1], uint primID : SV_PrimitiveID,inout TriangleStream<GS_TEXTUREVERTEX_OUTPUT> outStream)
{
    float halfWidth = input[0].size.x * 0.5f;
    float halfHeight = input[0].size.y * 0.5f;
    float4 pVertice[4];
    pVertice[0] = float4(input[0].center + halfWidth - halfHeight, 1.0f);
    pVertice[1] = float4(input[0].center + halfWidth + halfHeight, 1.0f);
    pVertice[2] = float4(input[0].center - halfWidth - halfHeight, 1.0f);
    pVertice[3] = float4(input[0].center - halfWidth + halfHeight, 1.0f);

    float2 pUVs[4] = { float2(0.0f, 1.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(1.0f, 0.0f) };

    GS_TEXTUREVERTEX_OUTPUT gsOut;
    for (int i = 0; i < 4; ++i)
    {
        gsOut.positionWorld = pVertice[i].xyz;
        gsOut.positionH = mul(mul(pVertice[i], gmtxView), gmtxProjection);
        gsOut.normal = float3(0.0f, 0.0f, -1.0f);
        gsOut.uv = pUVs[i];
        gsOut.primID = primID;
        outStream.Append(gsOut);
    }

}

float4 PSTextureVertex(GS_TEXTUREVERTEX_OUTPUT input) : SV_TARGET
{
    float3 uvw = float3(input.uv, (input.primID % 4));
    float4 cTexture = TextureArray.Sample(gSamplerState, uvw);
    float4 cColor = cTexture;
    return (cColor);
}