
#define NUM_CONTROL_POINTS 3

cbuffer cbPerObject : register(b0)
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

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]		: SV_TessFactor;
	float InsideTessFactor : SV_InsideTessFactor;
};

float3 PI(HS_OUT q, HS_OUT I)
{
	float3 q_minus_p = q.pos - I.pos;
	return q.pos - dot(q_minus_p, I.normal) * I.normal;
}

[domain("tri")]
DS_OUT main(HS_CONSTANT_DATA_OUTPUT hsConstData, float3 domainLocation : SV_DomainLocation, const OutputPatch<HS_OUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUT output = (DS_OUT)0;

	// The barycentric coordinates
	float fU = domainLocation.x;
	float fV = domainLocation.y;
	float fW = domainLocation.z;
	// Precompute squares 
	float fUU = fU * fU;
	float fVV = fV * fV;
	float fWW = fW * fW;

	float3 position = 
		patch[0].pos * fWW +
		patch[1].pos * fUU +
		patch[2].pos * fVV +
		fW * fU * (PI(patch[0], patch[1]) + PI(patch[1], patch[0])) +
		fU * fV * (PI(patch[1], patch[2]) + PI(patch[2], patch[1])) +
		fV * fW * (PI(patch[2], patch[0]) + PI(patch[0], patch[2]));

	float t = 0.5;
	position = position*t + (patch[0].pos * fW + patch[1].pos * fU + patch[2].pos * fV)*(1 - t);

	float3 normal = 
		patch[0].normal * fW +
		patch[1].normal * fU +
		patch[2].normal * fV;


	float4 p = mul(float4(position, 1.0f), viewMatrix);
	p = mul(p, projectionMatrix);

	output.pos = p;
	output.uv = patch[0].uv * fW + patch[1].uv * fU + patch[2].uv * fV;
	output.normal = normalize(normal);

	return output;
}
