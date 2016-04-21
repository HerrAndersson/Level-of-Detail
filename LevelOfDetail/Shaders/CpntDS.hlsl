/*----------------------------------------------------------------------------------------------------------------------
| Domain shader for the technique Curved PN Triangles																   |
----------------------------------------------------------------------------------------------------------------------*/

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
	DS_OUT output = (DS_OUT)0;

	//The barycentric coordinates
	float u = domainLocation.x;
	float v = domainLocation.y;
	float w = domainLocation.z;

	//Precompute squares 
	float uu = u * u;
	float vv = v * v;
	float ww = w * w;

	//Precompute squares * 3 
	float uu3 = uu * 3.0f;
	float vv3 = vv * 3.0f;
	float ww3 = ww * 3.0f;

	//Compute position from cubic control points and barycentric coords
	float3 position = 
		patch[0].pos.xyz * ww * w +
		patch[1].pos.xyz * uu * u +
		patch[2].pos.xyz * vv * v +
		hsConstData.B210 * ww3 * u +
		hsConstData.B120 * w * uu3 +
		hsConstData.B201 * ww3 * v +
		hsConstData.B021 * uu3 * v +
		hsConstData.B102 * w * vv3 +
		hsConstData.B012 * u * vv3 +
		hsConstData.B111 * 6.0f * w * u * v;

	//Compute normal from quadratic control points and barycentric coords
	float3 normal = 
		patch[0].normal * ww +
		patch[1].normal * uu +
		patch[2].normal * vv +
		hsConstData.N110 * w * u +
		hsConstData.N011 * u * v +
		hsConstData.N101 * w * v;

	float4 p = mul(float4(position, 1.0f), viewMatrix);
	p = mul(p, projectionMatrix);

	output.pos = p;
	output.uv = patch[0].uv * w + patch[1].uv * u + patch[2].uv * v;
	output.normal = normalize(normal);

	return output;
}
