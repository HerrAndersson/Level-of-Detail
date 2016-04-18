// Input control point
struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

// Output control point
struct HS_OUT
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

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_OUT, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT output;

	// Insert code to compute Output here
	output.EdgeTessFactor[0] = 
	output.EdgeTessFactor[1] = 
	output.EdgeTessFactor[2] = 
	output.InsideTessFactor = 15; // e.g. could calculate dynamic tessellation factors instead

	return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_OUT main(InputPatch<VS_OUT, NUM_CONTROL_POINTS> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
	HS_OUT output;

	// Insert code to compute Output here
	output.pos = ip[i].pos;
	output.uv = ip[i].uv;
	output.normal = ip[i].normal;

	return output;
}
