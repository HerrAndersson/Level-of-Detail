/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader used for rendering static geometry. Can handle texture or color.								           |	
----------------------------------------------------------------------------------------------------------------------*/
Texture2D diffuse : register(t0);
SamplerState samplerWrap : register(s0);

cbuffer bufferPerObject : register(b0)
{
	float3 color;
	float blendFactor;
	int hasTexture;
};

struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

float4 main(VS_OUT input) : SV_TARGET
{
	float4 finalColor;

	if (hasTexture > 1)
	{
		finalColor = diffuse.Sample(samplerWrap, input.uv), 1.0f;
	}
	else
	{
		finalColor = float4(color, 1.0f);
	}

	float3 lightDir = normalize(float3(0,100,0) - float3(0, 0, 100));
	finalColor = float4(finalColor * max(0, dot(input.normal, lightDir)));

	return float4(finalColor.xyz, blendFactor);
}