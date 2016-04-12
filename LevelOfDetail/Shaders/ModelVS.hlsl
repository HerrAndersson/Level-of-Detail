/*----------------------------------------------------------------------------------------------------------------------
| Vertex shader used to transform the static geometry to the screenspace for rendering.								   |
----------------------------------------------------------------------------------------------------------------------*/

cbuffer matrixBufferPerFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer matrixBufferPerObject : register(b1)
{
	matrix worldMatrix;
	float3 color;
	float blendFactor;
};

struct VS_IN
{
	float3 pos		: POSITION;
	float2 uv		: TEXCOORD;
	float3 normal	: NORMAL;
};

struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 color		: COLOR;
	float blendFactor   : BLENDFACTOR;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = mul(float4(input.pos, 1.0f), worldMatrix);
	pos = mul(pos, viewMatrix);
	pos = mul(pos, projectionMatrix);

	output.pos = pos;
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.uv = input.uv;
	output.color = color;
	output.blendFactor = blendFactor;
	
	return output;
}