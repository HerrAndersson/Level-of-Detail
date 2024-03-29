/*----------------------------------------------------------------------------------------------------------------------
| Vertex shader used to transform the static geometry to the screenspace for rendering.								   |
----------------------------------------------------------------------------------------------------------------------*/

cbuffer bufferPerFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer bufferPerObject : register(b1)
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
	float4 pos			: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = mul(float4(input.pos, 1.0f), worldMatrix);
	pos = mul(pos, viewMatrix);
	pos = mul(pos, projectionMatrix);

	output.pos = pos;
	output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
	output.uv = input.uv;
	
	return output;
}