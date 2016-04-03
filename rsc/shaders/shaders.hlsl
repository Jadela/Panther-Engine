cbuffer PerFrame : register(b0)
{
	matrix mvp;
}

struct PSInput
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : UV;
};

Texture2D		g_txDiffuse : register(t0);
SamplerState	g_sampler : register(s0);

PSInput VSMain(float3 position : POSITION, float3 normal : NORMAL, float4 color : COLOR, float2 uv : UV)
{
	PSInput result;

	result.position = mul(mvp, float4(position, 1.0f));
	result.normal = mul(mvp, float4(normal, 0.0f));
	result.color = color;
	result.uv = uv;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float ambient = 0.1f;
	float diffuse = max(dot(input.normal, float4(0.707, 0, -0.707, 0)), 0.0f);
	float illum = ambient + diffuse;

	return g_txDiffuse.Sample(g_sampler, input.uv) * illum;
	return input.color;
}
