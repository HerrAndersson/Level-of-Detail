/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader used for the rendering of static geometry.															   |	
----------------------------------------------------------------------------------------------------------------------*/
Texture2D diffuse : register(t0);

SamplerState samplerWrap : register(s0);

struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

float4 main(VS_OUT input) : SV_TARGET
{
	return float4(0.2f, 0.4f, 0.2f, 1.0f);
	//float4 color = diffuse.Sample(samplerWrap, input.uv);

	//return float4(color.xyz, 1.0f);
}