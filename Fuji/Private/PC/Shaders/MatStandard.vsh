#include "Registers.h"

float4x4 mWorldToScreen : register(c_wvp);
float4x4 mLocalToWorld : register(c_ltw);
float4 mAnimMats[70*3] : register(c_animMats);
float4 mTexMatrix[2] : register(c_tex);

float4 gModelColour : register(c_modelColour);
float4 gColourMask : register(c_colourMask);
float4 bAnimation[2] : register(c_animating);

int gNumWeights : register(c_numWeights);


struct VS_INPUT
{
	float4 pos		: POSITION;
	float3 norm		: NORMAL;
	float4 uv		: TEXCOORD; 
	float4 colour	: COLOR0;
	float4 weights	: BLENDWEIGHT;
	float4 indices	: BLENDINDICES;
};

struct VS_OUTPUT
{
    float4 pos		: POSITION;
    float4 colour	: COLOR0;
    
    float4 uv		: TEXCOORD;
};

VS_OUTPUT main(in VS_INPUT input)
{
    VS_OUTPUT output;

	float4 pos = input.pos * bAnimation[0];
/*
	// animate
	for(int a=0; a<gNumWeights; a++)
	{
		int i = input.indices[a];
		float w = input.weights[a];

		float4 t = float4(0,0,0,0);
		t.x = dot(input.pos, mAnimMats[i]);
		t.y = dot(input.pos, mAnimMats[i+1]);
		t.z = dot(input.pos, mAnimMats[i+2]);

		pos += t*w;
	}
*/

	int i;
	float3 t0, t1, t2, t3;
	input.indices = input.indices * 255 + float4(0.5, 0.5, 0.5, 0.5);

	i = input.indices.x;
	t0.x = dot(input.pos, mAnimMats[i]);
	t0.y = dot(input.pos, mAnimMats[i+1]);
	t0.z = dot(input.pos, mAnimMats[i+2]);
	i = input.indices.y;
	t1.x = dot(input.pos, mAnimMats[i]);
	t1.y = dot(input.pos, mAnimMats[i+1]);
	t1.z = dot(input.pos, mAnimMats[i+2]);
	i = input.indices.z;
	t2.x = dot(input.pos, mAnimMats[i]);
	t2.y = dot(input.pos, mAnimMats[i+1]);
	t2.z = dot(input.pos, mAnimMats[i+2]);
	i = input.indices.w;
	t3.x = dot(input.pos, mAnimMats[i]);
	t3.y = dot(input.pos, mAnimMats[i+1]);
	t3.z = dot(input.pos, mAnimMats[i+2]);

	pos.xyz += (t0*input.weights.x + t1*input.weights.y + t2*input.weights.z + t3*input.weights.w) * bAnimation[1];
	// do lighting

	// transform pos
	output.pos = mul(pos, mWorldToScreen);

	// apply texture matrix
	output.uv.x = dot(input.uv, mTexMatrix[0]);
	output.uv.yzw = dot(input.uv, mTexMatrix[1]);

	// output colour and apply colour mask
	output.colour.xyz = input.colour.xyz*gModelColour.xyz*gColourMask.x + gColourMask.yyy;
	output.colour.w = input.colour.w*gModelColour.w*gColourMask.z + gColourMask.w;

    return output;
}

