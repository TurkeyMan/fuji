// fuji matrices
float4x4 mWorld : register(c0);
float4x4 mCamera : register(c4);
float4x4 mProjection : register(c8);
float4x4 mShadowMap : register(c12);
float4x4 mFuji0 : register(c16);
float4x4 mFuji1 : register(c20);
float4x4 mUser0 : register(c24);
float4x4 mUser1 : register(c28);
float4x4 mUser2 : register(c32);
float4x4 mUser3 : register(c36);
float4x4 mUV0 : register(c40);
float4x4 mUV1 : register(c44);
float4x4 mUV2 : register(c48);
float4x4 mUV3 : register(c52);
float4x4 mView : register(c56);
float4x4 mWorldView : register(c60);
float4x4 mViewProjection : register(c64);
float4x4 mWorldViewProjection : register(c68);
float4x4 mInverseWorld : register(c72);
float4x4 mInverseViewProjection : register(c76);

// fuji vectors
float4 vTime : register(c80);
float4 vFogColour : register(c81);
float4 vFogParams1 : register(c82);
float4 vFogParams2 : register(c83);
float4 vRenderState : register(c84);
float4 vMaterialDiffuseColour : register(c85);
float4 vDiffuseColour : register(c86);
float4 vAmbientColour : register(c87);
float4 vFuji0 : register(c88);
float4 vFuji1 : register(c89);
float4 vFuji2 : register(c90);
float4 vFuji3 : register(c91);
float4 vFuji4 : register(c92);
float4 vFuji5 : register(c93);
float4 vFuji6 : register(c94);
float4 vLightCounts : register(c95);
float4 vUser0 : register(c96);
float4 vUser1 : register(c97);
float4 vUser2 : register(c98);
float4 vUser3 : register(c99);
float4 vUser4 : register(c100);
float4 vUser5 : register(c101);
float4 vUser6 : register(c102);
float4 vUser7 : register(c103);
float4 vUser8 : register(c104);
float4 vUser9 : register(c105);
float4 vUser10 : register(c106);
float4 vUser11 : register(c107);
float4 vUser12 : register(c108);
float4 vUser13 : register(c109);
float4 vUser14 : register(c110);
float4 vUser15 : register(c111);

float4 mAnimationMatrices[48*3] : register(c112);

// integer values
int4 iLightCounts : register(i0);
int4 iAnimationParams : register(i1);

// fuji bools
bool bAnimated : register(b0);
bool bZPrime : register(b1);
bool bShadowGeneration : register(b2);
bool bShadowReceiving : register(b3);
bool bOpaque : register(b4);
bool bAlphaTest : register(b5);
bool bFuji0 : register(b6);
bool bFuji1 : register(b7);
bool bFuji2 : register(b8);
bool bUser0 : register(b9);
bool bUser1 : register(b10);
bool bUser2 : register(b11);
bool bUser3 : register(b12);
bool bDiffuseSet : register(b13);
bool bNormalMapSet : register(b14);
bool bSpecularMapSet : register(b15);
bool bDetailMapSet : register(b16);
bool bOpacityMapSet : register(b17);
bool bEnvironmentMapSet : register(b18);
bool bSpecularPowerMapSet : register(b19);
bool bEmissiveMapSet : register(b20);
bool bLightMapSet : register(b21);
bool bShadowBufferSet : register(b22);
bool bProjectorSet : register(b23);
bool bUserTex0Set : register(b24);
bool bUserTex1Set : register(b25);
bool bUserTex2Set : register(b26);
bool bUserTex3Set : register(b27);
bool bUserTex4Set : register(b28);
bool bVertexTex0Set : register(b29);
bool bVertexTex1Set : register(b30);
bool bVertexTex2Set : register(b31);

// fuji samplers
sampler2D sDiffuseSampler : register(s0);
sampler2D sNormalSampler : register(s1);
sampler2D sSpecularSampler : register(s2);
sampler2D sDetailSampler : register(s3);
sampler2D sOpacitySampler : register(s4);
sampler2D sEnvironmentSampler : register(s5);
sampler2D sSpecularPowerSampler : register(s6);
sampler2D sEmissiveSampler : register(s7);
sampler2D sLightSampler: register(s8);
sampler2D sShadowBufferSampler : register(s9);
sampler2D sProjectionSampler : register(s10);
sampler2D sUser0Sampler : register(s11);
sampler2D sUser1Sampler : register(s12);
sampler2D sUser2Sampler : register(s13);
sampler2D sUser3Sampler : register(s14);
sampler2D sUser4Sampler : register(s15);
//sampler2D sVertex0Sampler : register(s0);
//sampler2D sVertex1Sampler : register(s1);
//sampler2D sVertex2Sampler : register(s2);

struct StaticInput
{
	float4 pos		: POSITION;
	float3 norm		: NORMAL;
	float4 uv		: TEXCOORD;
	float4 colour	: COLOR0;
};

struct AnimatedInput
{
	float4 pos		: POSITION;
	float3 norm		: NORMAL;
	float4 uv		: TEXCOORD;
	float4 colour	: COLOR0;
	float4 weights	: BLENDWEIGHT;
	float4 indices	: BLENDINDICES;
};

struct VSOutput
{
	float4 pos		: POSITION;
	float4 colour	: COLOR0;
	float2 uv		: TEXCOORD0;
};


float4 animate(float4 pos, int4 indices, float4 weights, int numWeights)
{
//	indices *= 3;

	float4 newPos = pos;
	if(numWeights > 0)
	{
		int i = indices.x;
		float3 t;
		t.x = dot(pos, mAnimationMatrices[i]);
		t.y = dot(pos, mAnimationMatrices[i+1]);
		t.z = dot(pos, mAnimationMatrices[i+2]);
		newPos.xyz = t*weights.x;
	}
	if(numWeights > 1)
	{
		int i = indices.y;
		float3 t;
		t.x = dot(pos, mAnimationMatrices[i]);
		t.y = dot(pos, mAnimationMatrices[i+1]);
		t.z = dot(pos, mAnimationMatrices[i+2]);
		newPos.xyz += t*weights.y;
	}
	if(numWeights > 2)
	{
		int i = indices.z;
		float3 t;
		t.x = dot(pos, mAnimationMatrices[i]);
		t.y = dot(pos, mAnimationMatrices[i+1]);
		t.z = dot(pos, mAnimationMatrices[i+2]);
		newPos.xyz += t*weights.z;
	}
	if(numWeights > 3)
	{
		int i = indices.w;
		float3 t;
		t.x = dot(pos, mAnimationMatrices[i]);
		t.y = dot(pos, mAnimationMatrices[i+1]);
		t.z = dot(pos, mAnimationMatrices[i+2]);
		newPos.xyz += t*weights.w;
	}

	return newPos;
}

float2 transformUV(float4 uv, int uvMatrix)
{
	uvMatrix *= 2;

	float2 t;
	if(uvMatrix == 0)
	{
		t.x = dot(uv, mUV0._m00_m10_m20_m30); // TODO: look into why the matrices are sideways...
		t.y = dot(uv, mUV0._m01_m11_m21_m31);
	}
	else if(uvMatrix == 1)
	{
		t.x = dot(uv, mUV1._m00_m10_m20_m30);
		t.y = dot(uv, mUV1._m01_m11_m21_m31);
	}
	else if(uvMatrix == 2)
	{
		t.x = dot(uv, mUV2._m00_m10_m20_m30);
		t.y = dot(uv, mUV2._m01_m11_m21_m31);
	}
	else if(uvMatrix == 3)
	{
		t.x = dot(uv, mUV3._m00_m10_m20_m30);
		t.y = dot(uv, mUV3._m01_m11_m21_m31);
	}
	return t;
}
