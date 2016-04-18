// Output control point
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
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUT main(HS_CONSTANT_DATA_OUTPUT input, float3 domain : SV_DomainLocation, const OutputPatch<HS_OUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUT output;

	output.pos = patch[0].pos;
	output.uv = patch[0].uv;
	output.normal = patch[0].normal;

	return output;
}
