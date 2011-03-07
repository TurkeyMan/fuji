#include "Registers.h"

float4x4 mWorldToScreen : register(c_wvp);
float4x4 mLocalToWorld : register(c_ltw);
float4 mTexMatrix[2] : register(c_tex);

float4 gModelColour : register(c_modelColour);
float4 gColourMask : register(c_colourMask);

cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;
    float4 vMeshColor;
};


struct VS_INPUT
{
	float4 pos		: POSITION;
	float3 norm		: NORMAL;
	float4 uv		: TEXCOORD; 
	float4 colour	: COLOR0;

};

struct VS_OUTPUT
{
    float4 pos		: SV_POSITION;
    float4 colour	: COLOR0;
    
    float4 uv		: TEXCOORD0;
    float4 uv2		: TEXCOORD1;
};

VS_OUTPUT main(in VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

	float4 pos = input.pos;

	// do lighting

	// transform pos
	output.pos = mul(pos, mWorldToScreen);

	// apply texture matrix
	output.uv.x = dot(input.uv, mTexMatrix[0]);
	output.uv.yzw = dot(input.uv, mTexMatrix[1]);
	output.uv2 = output.uv;

	// output colour and apply colour mask
	output.colour.xyz = input.colour.xyz*gModelColour.xyz*gColourMask.x + gColourMask.yyy;
	output.colour.w = input.colour.w*gModelColour.w*gColourMask.z + gColourMask.w;

    return output;
}

