#include "ConstantBuffers.hlsli"
#include "Lighting.hlsli"

Texture2D diffuseTexture : register(t0);
SamplerState defaultSampler : register(s0);

struct VtP
{
	float4 Position : SV_POSITION;
	float3 Normal_WS : NORMAL;
	float4 Color : COLOR;
	float2 UV : TEXCOORD;
	float4 Pos_WS : POSITION;
};


VtP VSMain(float3 Position : POSITION, float3 Normal : NORMAL, float4 Color : COLOR, float2 UV : TEXCOORD)
{
	VtP output;

	output.Position = mul(m_MVP, float4(Position, 1));
	output.Normal_WS = mul((float3x3)m_IT_M, Normal);
	output.Color = Color;
	output.UV = UV;
	output.Pos_WS = mul(m_M, float4(Position, 1));

	return output;
}

LightingOutput ComputeLighting(float4 Pos_WS, float3 Normal)
{
	float3 CameraDirection = normalize(m_CameraPosition.xyz - Pos_WS.xyz);

	LightingOutput result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	result.Diffuse = CalculateDiffuse(m_Light0Direction.xyz, Normal);
	result.Specular = CalculateSpecular(CameraDirection, m_Light0Direction.xyz, Normal);

	return result;
}

float4 PSMain(VtP input) : SV_TARGET
{
	LightingOutput lighting = ComputeLighting(input.Pos_WS, normalize(input.Normal_WS));
	
	float day = m_Light0Direction.y;

	float4 emissive = { 0, 0, 0, 0 };
	float4 ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
	float4 diffuse = lighting.Diffuse;
	float4 specular = lighting.Specular;
	float4 illum = ambient + diffuse + specular;

	return illum * diffuseTexture.Sample(defaultSampler, input.UV);
}
