/*----------------------------------------------------------------------------------------------------------------------
| Hull shader for the technique Phong Tessellation																	   |
----------------------------------------------------------------------------------------------------------------------*/
#define NUM_CONTROL_POINTS 3

cbuffer cbPerPatch : register(b0)
{
	float3 cameraPosition;
	float tessellationFactor;
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
};

//Returns a distance adaptive tessellation scale factor (0.0f -> 1.0f) 
float GetDistanceAdaptiveScaleFactor(float3 cameraPos, float3 edgePos0, float3 edgePos1, float minDistance, float range)
{
	float3 midPoint = (edgePos0 + edgePos1) * 0.5f;
	float dist = distance(midPoint, cameraPos) - minDistance;
	float scale = 1.0f - saturate(dist / range);

	return scale;
}

//Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_OUT, NUM_CONTROL_POINTS> inputPatch, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT output;

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
	output.InsideTessFactor = (output.EdgeTessFactor[0] + output.EdgeTessFactor[1] + output.EdgeTessFactor[2]) / 3.0f;

	return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[patchconstantfunc("CalcHSPatchConstants")]
[outputcontrolpoints(3)]
[maxtessfactor(15.0f)]
HS_OUT main(InputPatch<VS_OUT, NUM_CONTROL_POINTS> inputPatch, uint cpid : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	HS_OUT output = (HS_OUT)0;

	//Passthrough
	output.pos = inputPatch[cpid].pos;
	output.uv = inputPatch[cpid].uv;
	output.normal = inputPatch[cpid].normal;

	return output;
}
