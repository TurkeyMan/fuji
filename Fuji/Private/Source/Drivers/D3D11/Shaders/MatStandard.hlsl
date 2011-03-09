
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );


cbuffer cbEverything : register( b0 )
{
	matrix mWorldToScreen;
	matrix mLocalToWorld;
	float4 mTexMatrix[2];
    float4 vMeshColor;
	float4 gModelColour;
	float4 gColourMask;
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

VS_OUTPUT vs_main(in VS_INPUT input)
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
	output.colour.xyz = input.colour.xyz * gModelColour.xyz * gColourMask.x + gColourMask.yyy;
	output.colour.w = input.colour.w * gModelColour.w * gColourMask.z + gColourMask.w;

    return output;
}

float4 ps_main( VS_OUTPUT input) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.uv ) * input.colour;
}
