// Vertex shader
cbuffer PerObject : register(b0)
{
	matrix InverseTransposeWorldMatrix;
	matrix MVP;
}

struct VtP
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float4 Color : COLOR;
	float2 UV : TEXCOORD;
};


VtP VSMain(float3 Position : POSITION, float3 Normal : NORMAL, float4 Color : COLOR, float2 UV : TEXCOORD)
{
	VtP output;

	output.Position = mul(MVP, float4(Position, 1.0f));
	output.Normal = mul((float3x3)InverseTransposeWorldMatrix, Normal);
	output.Color = Color;
	output.UV = UV;

	return output;
}

// Pixel shader
cbuffer PerPass : register(b1)
{
	float3 LightDirection;
}

Texture2D		diffuseTexture : register(t0);
SamplerState	defaultSampler : register(s0);

struct LightingOutput
{
	float4 Diffuse;
};

float4 CalculateDiffuse()
{

}

LightingOutput ComputeLighting(float4 Pos_WS, float3 Normal)
{
	LightingOutput result = { 0, 0, 0, 0 };

	return result;
}

float4 PSMain(VtP input) : SV_TARGET
{
	float day = LightDirection.y;
	float light = max(dot(input.Normal, LightDirection), 0.0f);

	float4 emissive = { 0, 0, 0, 0 };
	float4 ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
	float4 diffuse = { light, light, light, light };
	float4 illum = ambient + diffuse;

	return illum * diffuseTexture.Sample(defaultSampler, input.UV);
}
