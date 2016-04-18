
#define NUM_CONTROL_POINTS 3

cbuffer bufferPerObject : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct HS_OUT
{
	float4 pos			: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

struct DS_OUT
{
	float4 pos			: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

[domain("tri")]
DS_OUT main(HS_CONSTANT_DATA_OUTPUT input, float3 domain : SV_DomainLocation, const OutputPatch<HS_OUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUT output;

	float4 p = domain.x * patch[0].pos + domain.y * patch[1].pos + domain.z * patch[2].pos;

	output.pos = mul(p, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	output.uv = patch[0].uv;
	output.normal = patch[0].normal;

	return output;
}
