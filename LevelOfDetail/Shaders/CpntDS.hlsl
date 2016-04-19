
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
	float InsideTessFactor		: SV_InsideTessFactor;

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

[domain("tri")]
DS_OUT main(HS_CONSTANT_DATA_OUTPUT hsConstData, float3 domainLocation : SV_DomainLocation, const OutputPatch<HS_OUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUT output;

	// The barycentric coordinates
	float fU = domainLocation.x;
	float fV = domainLocation.y;
	float fW = domainLocation.z;

	// Precompute squares 
	float fUU = fU * fU;
	float fVV = fV * fV;
	float fWW = fW * fW;

	// Precompute squares * 3 
	float fUU3 = fUU * 3.0f;
	float fVV3 = fVV * 3.0f;
	float fWW3 = fWW * 3.0f;

	// Compute position from cubic control points and barycentric coords
	float3 position = 
		patch[0].pos.xyz * fWW * fW +
		patch[1].pos.xyz * fUU * fU +
		patch[2].pos.xyz * fVV * fV +
		hsConstData.B210 * fWW3 * fU +
		hsConstData.B120 * fW * fUU3 +
		hsConstData.B201 * fWW3 * fV +
		hsConstData.B021 * fUU3 * fV +
		hsConstData.B102 * fW * fVV3 +
		hsConstData.B012 * fU * fVV3 +
		hsConstData.B111 * 6.0f * fW * fU * fV;

	// Compute normal from quadratic control points and barycentric coords
	float3 normal = 
		patch[0].normal * fWW +
		patch[1].normal * fUU +
		patch[2].normal * fVV +
		hsConstData.N110 * fW * fU +
		hsConstData.N011 * fU * fV +
		hsConstData.N101 * fW * fV;

	normal = normalize(normal);

	float4 p = mul(float4(position, 1.0f), viewMatrix);
	p = mul(p, projectionMatrix);

	output.pos = p;
	output.uv = patch[0].uv * fW + patch[1].uv * fU + patch[2].uv * fV;
	output.normal = normal;

	return output;
}
