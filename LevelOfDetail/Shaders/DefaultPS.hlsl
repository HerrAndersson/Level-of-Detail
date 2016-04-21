/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader used for rendering static geometry. Can handle texture or color.								           |	
----------------------------------------------------------------------------------------------------------------------*/
Texture2D diffuse : register(t0);

SamplerState samplerWrap : register(s0);

cbuffer bufferPerObject : register(b0)
{
	float3 color;
	float blendFactor;
};

struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

float4 main(VS_OUT input) : SV_TARGET
{
	//float3 color = diffuse.Sample(samplerWrap, input.uv);

	float3 lightDir = normalize(float3(0,100,0) - float3(0, 0, 100));
	float lightIntensity = saturate(dot(input.normal, lightDir));
	float4 finalColor = saturate(float4(color, 1.0f) * lightIntensity);

	distance(input.pos.xyz, input.normal);

	return float4(finalColor.xyz, blendFactor);
}