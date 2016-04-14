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
	float3 color		: COLOR;
	float blendFactor   : BLENDFACTOR;
};

float4 main(VS_OUT input) : SV_TARGET
{
	float3 lightDir = normalize(float3(0,100,0) - float3(0, 0, 100));
	float lightIntensity = saturate(dot(input.normal.xyz, lightDir));
	float4 finalColor = saturate(float4(input.color, 1.0f) * lightIntensity);

	return float4(finalColor.xyz, input.blendFactor);

	//float4 color = diffuse.Sample(samplerWrap, input.uv);
	//return float4(color.xyz, 1.0f);
}