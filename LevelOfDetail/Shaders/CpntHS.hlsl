/*----------------------------------------------------------------------------------------------------------------------
| Hull shader for the technique Curved PN Triangles																	   |
----------------------------------------------------------------------------------------------------------------------*/
#include "TessellationHelpers.hlsli"
#define NUM_CONTROL_POINTS 3

cbuffer cbPerPatch : register(b0)
{
	float3 cameraPosition;
	float tessellationFactor;
	float3 viewVector;
	float minDistance;
	float range;
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
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_OUT, NUM_CONTROL_POINTS> inputPatch, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT output = (HS_CONSTANT_DATA_OUTPUT)0;

	// Aquire patch edge dot product between patch edge normal and view vector 
	float edgeDot[3];
	edgeDot[0] = GetEdgeDotProduct(inputPatch[2].normal, inputPatch[0].normal, normalize(viewVector.xyz));
	edgeDot[1] = GetEdgeDotProduct(inputPatch[0].normal, inputPatch[1].normal, normalize(viewVector.xyz));
	edgeDot[2] = GetEdgeDotProduct(inputPatch[1].normal, inputPatch[2].normal, normalize(viewVector.xyz));

	//If all 3 fail the test then back face cull
	if (BackFaceCull(edgeDot[0], edgeDot[1], edgeDot[2], 0.5f) == true)
	{
		// Cull the patch (all the tess factors are set to 0)
		return output;
	}

	output.EdgeTessFactor[0] = output.EdgeTessFactor[1] = output.EdgeTessFactor[2] = tessellationFactor;

	//Perform distance adaptive tessellation per edge
	//Edge 0
	float adaptiveScaleFactor = GetDistanceAdaptiveScaleFactor(cameraPosition, inputPatch[2].pos.xyz, inputPatch[0].pos.xyz, minDistance, range);
	output.EdgeTessFactor[0] = lerp(1.0f, output.EdgeTessFactor[0], adaptiveScaleFactor);
	//Edge 1
	adaptiveScaleFactor = GetDistanceAdaptiveScaleFactor(cameraPosition, inputPatch[0].pos.xyz, inputPatch[1].pos.xyz, minDistance, range);
	output.EdgeTessFactor[1] = lerp(1.0f, output.EdgeTessFactor[1], adaptiveScaleFactor);
	//Edge 2
	adaptiveScaleFactor = GetDistanceAdaptiveScaleFactor(cameraPosition, inputPatch[1].pos.xyz, inputPatch[2].pos.xyz, minDistance, range);
	output.EdgeTessFactor[2] = lerp(1.0f, output.EdgeTessFactor[2], adaptiveScaleFactor);

	//Inside tess factor is the average of the three edge factors
	output.InsideTessFactor = (output.EdgeTessFactor[0] + output.EdgeTessFactor[1] + output.EdgeTessFactor[2]) / 3.0f;;

	//Assign Positions and normals
	float3 B003 = inputPatch[0].pos.xyz;
	float3 B030 = inputPatch[1].pos.xyz;
	float3 B300 = inputPatch[2].pos.xyz;
	
	float3 N002 = inputPatch[0].normal;
	float3 N020 = inputPatch[1].normal;
	float3 N200 = inputPatch[2].normal;

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

	// Compute the quadratic normal control points, and rotate into world space
	float V12 = 2.0f * dot(B030 - B003, N002 + N020) / dot(B030 - B003, B030 - B003);
	output.N110 = normalize(N002 + N020 - V12 * (B030 - B003));
	float V23 = 2.0f * dot(B300 - B030, N020 + N200) / dot(B300 - B030, B300 - B030);
	output.N011 = normalize(N020 + N200 - V23 * (B300 - B030));
	float V31 = 2.0f * dot(B003 - B300, N200 + N002) / dot(B003 - B300, B003 - B300);
	output.N101 = normalize(N200 + N002 - V31 * (B003 - B300));

	return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[patchconstantfunc("CalcHSPatchConstants")]
[outputcontrolpoints(3)]
[maxtessfactor(15.0f)]
HS_OUT main(InputPatch<VS_OUT, NUM_CONTROL_POINTS> inputPatch, uint cpid : SV_OutputControlPointID, uint patchID : SV_PrimitiveID )
{
	HS_OUT output = (HS_OUT)0;

	//Passthrough
	output.pos = inputPatch[cpid].pos;
	output.uv = inputPatch[cpid].uv;
	output.normal = inputPatch[cpid].normal;

	return output;
}
