/*----------------------------------------------------------------------------------------------------------------------
| Hull shader for the technique Phong Tessellation																	   |
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
};

//Patch Constant Function
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

	// Scale the tessellation factors based on patch orientation with respect to the viewing vector
	// Edge 0
	float adaptiveScaleFactor = GetOrientationAdaptiveScaleFactor(edgeDot[0], 0.25f) * GetDistanceAdaptiveScaleFactor(cameraPosition, inputPatch[2].pos.xyz, inputPatch[0].pos.xyz, minDistance, range);;
	float tess0 = lerp(1.0f, tessellationFactor, adaptiveScaleFactor);
	// Edge 1
	adaptiveScaleFactor = GetOrientationAdaptiveScaleFactor(edgeDot[1], 0.25f) * GetDistanceAdaptiveScaleFactor(cameraPosition, inputPatch[0].pos.xyz, inputPatch[1].pos.xyz, minDistance, range);;
	float tess1 = lerp(1.0f, tessellationFactor, adaptiveScaleFactor);
	// Edge 2
	adaptiveScaleFactor = GetOrientationAdaptiveScaleFactor(edgeDot[2], 0.25f) * GetDistanceAdaptiveScaleFactor(cameraPosition, inputPatch[1].pos.xyz, inputPatch[2].pos.xyz, minDistance, range);;
	float tess2 = lerp(1.0f, tessellationFactor, adaptiveScaleFactor);

	output.EdgeTessFactor[0] = tess0;
	output.EdgeTessFactor[1] = tess1;
	output.EdgeTessFactor[2] = tess2;

	//Inside tess factor is the average of the three edge factors
	output.InsideTessFactor = (output.EdgeTessFactor[0] + output.EdgeTessFactor[1] + output.EdgeTessFactor[2]) / 3.0f;

	////Minimize the increase---------------------------------------------------------------
	////Depending on distance, clamp to 2 when far away?
	//output.InsideTessFactor = clamp(output.InsideTessFactor, 1.0f, 3.0f);
	//output.EdgeTessFactor[0] = output.EdgeTessFactor[1] = output.EdgeTessFactor[2] = 1.0f;
	////------------------------------------------------------------------------------------

	return output;
}

[domain("tri")]
[partitioning("integer")]
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
