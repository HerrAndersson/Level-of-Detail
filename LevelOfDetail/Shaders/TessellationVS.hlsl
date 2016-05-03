/*----------------------------------------------------------------------------------------------------------------------
| Vertex shader used when an object is rendered with tessellation active.											   |
----------------------------------------------------------------------------------------------------------------------*/

cbuffer cbPerObject : register(b1)
{
	matrix worldMatrix;
};

struct VS_IN
{
	float3 pos		: POSITION;
	float2 uv		: TEXCOORD;
	float3 normal	: NORMAL;
};

struct VS_OUT
{
	float4 pos			: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = mul(float4(input.pos, 1.0f), worldMatrix);

	output.pos = pos;

	output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
	output.uv = input.uv;

	return output;
}