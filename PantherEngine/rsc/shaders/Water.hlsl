#include "ConstantBuffers.hlsli"
#include "Lighting.hlsli"

Texture2D waterTexture : register(t0);
SamplerState defaultSampler : register(s0);

struct VtP
{
	float4 Position : SV_POSITION;
	float3 Normal_WS : NORMAL;
	float2 UV : TEXCOORD;
	float4 Pos_WS : POSITION;
};

VtP VSMain(float3 Position : POSITION, float3 Normal : NORMAL, float2 UV : TEXCOORD)
{
	VtP output;

	output.Position = mul(m_MVP, float4(Position, 1));
	output.Normal_WS = normalize(mul((float3x3)m_IT_M, Normal));
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
	float3 normal = (waterTexture.Sample(defaultSampler, input.UV + m_Time).xyz + waterTexture.Sample(defaultSampler, input.UV - m_Time).xyz + waterTexture.Sample(defaultSampler, input.UV + float2(m_Time, -m_Time)).xyz) / 3.0;
	normal = (normal * 2) - 1; // Expand from range [0,1] to [-1,1]
	normal = normalize(mul((float3x3) m_M, normal)); // Texture to world space

	LightingOutput lighting = ComputeLighting(input.Pos_WS, normal);

	float4 emissive = { 0, 0, 0, 0 };
	float4 ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
	float4 diffuse = lighting.Diffuse;
	float4 specular = lighting.Specular;
	float4 illum = ambient + diffuse + specular;

	return illum * float4(0.39f, 0.58f, 0.93f, 1);
}