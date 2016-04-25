cbuffer PerObject : register(b0)
{
	matrix InverseTransposeWorldMatrix;
	matrix MVP;
}

cbuffer PerPass : register(b1)
{
	float3 LightDirection;
}

struct VtP
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : UV;
};

Texture2D		g_txDiffuse : register(t0);
SamplerState	g_sampler : register(s0);

VtP VSMain(float3 position : POSITION, float3 normal : NORMAL, float4 color : COLOR, float2 uv : UV)
{
	VtP output;

	output.position = mul(MVP, float4(position, 1.0f));
	output.normal = mul((float3x3)InverseTransposeWorldMatrix, normal);
	output.color = color;
	output.uv = uv;

	return output;
} 

float4 PSMain(VtP input) : SV_TARGET
{
	float ambient = 0.1f;
	float day = LightDirection.y;
	float diffuse = max(dot(input.normal, LightDirection), 0.0f);
	float illum = ambient + diffuse;

	return g_txDiffuse.Sample(g_sampler, input.uv) * illum;
}
