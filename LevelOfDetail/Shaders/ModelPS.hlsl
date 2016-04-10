/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader used for the rendering of static geometry.															   |	
----------------------------------------------------------------------------------------------------------------------*/
Texture2D diffuse : register(t0);

SamplerState samplerWrap : register(s0);

struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{
	float4 color = diffuse.Sample(samplerWrap, input.uv);

	return float4(color.xyz, 1.0f);
}