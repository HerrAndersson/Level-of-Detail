#define NUM_CONTROL_POINTS 3

cbuffer cbPerPatch : register(b0)
{
	float distanceToCamera;
	float tessellationFactor;
};

struct VS_OUT
{
	float4 pos			: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

struct HS_OUT
{
	float4 pos			: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;

	//Geometry cubic generated control points
	float3 B210    : POSITION3;
	float3 B120    : POSITION4;
	float3 B021    : POSITION5;
	float3 B012    : POSITION6;
	float3 B102    : POSITION7;
	float3 B201    : POSITION8;
	float3 B111    : CENTER;

	//Normal quadratic generated control points
	float3 N110    : NORMAL3;
	float3 N011    : NORMAL4;
	float3 N101    : NORMAL5;
};

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_OUT, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT output;

	output.EdgeTessFactor[0] = 
	output.EdgeTessFactor[1] = 
	output.EdgeTessFactor[2] = 
	output.InsideTessFactor = tessellationFactor / (distanceToCamera / 10.0f);

	//Assign Positions and normals
	float3 B003 = ip[0].pos.xyz;
	float3 B030 = ip[1].pos.xyz;
	float3 B300 = ip[2].pos.xyz;

	float3 N002 = ip[0].normal;
	float3 N020 = ip[1].normal;
	float3 N200 = ip[2].normal;

	//Compute the cubic geometry control points
	//Edge control points
	output.B210 = ((2.0f * B003) + B030 - (dot((B030 - B003), N002) * N002)) / 3.0f;
	output.B120 = ((2.0f * B030) + B003 - (dot((B003 - B030), N020) * N020)) / 3.0f;
	output.B021 = ((2.0f * B030) + B300 - (dot((B300 - B030), N020) * N020)) / 3.0f;
	output.B012 = ((2.0f * B300) + B030 - (dot((B030 - B300), N200) * N200)) / 3.0f;
	output.B102 = ((2.0f * B300) + B003 - (dot((B003 - B300), N200) * N200)) / 3.0f;
	output.B201 = ((2.0f * B003) + B300 - (dot((B300 - B003), N002) * N002)) / 3.0f;

	//Center control point
	float3 E = (output.B210 + output.B120 + output.B021 + output.B012 + output.B102 + output.B201) / 6.0f;
	float3 V = (B003 + B030 + B300) / 3.0f;
	output.B111 = E + ((E - V) / 2.0f);

	//Compute the quadratic normal control points, and rotate into world space
	float fV12 = 2.0f * dot(B030 - B003, N002 + N020) / dot(B030 - B003, B030 - B003);
	output.N110 = normalize(N002 + N020 - fV12 * (B030 - B003));
	float fV23 = 2.0f * dot(B300 - B030, N020 + N200) / dot(B300 - B030, B300 - B030);
	output.N011 = normalize(N020 + N200 - fV23 * (B300 - B030));
	float fV31 = 2.0f * dot(B003 - B300, N200 + N002) / dot(B003 - B300, B003 - B300);
	output.N101 = normalize(N200 + N002 - fV31 * (B003 - B300));

	return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[patchconstantfunc("CalcHSPatchConstants")]
[outputcontrolpoints(3)]
//[maxtessfactor(15.0f)]
HS_OUT main(InputPatch<VS_OUT, NUM_CONTROL_POINTS> inputPatch, uint cpid : SV_OutputControlPointID, uint patchID : SV_PrimitiveID )
{
	HS_OUT output = (HS_OUT)0;

	output.pos = inputPatch[cpid].pos;
	output.uv = inputPatch[cpid].uv;
	output.normal = inputPatch[cpid].normal;

	return output;
}
