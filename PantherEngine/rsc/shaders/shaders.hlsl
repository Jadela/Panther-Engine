// Vertex shader
cbuffer VertexConstants : register(b0)
{
	matrix WorldMatrix;
	matrix InverseTransposeWorldMatrix;
	matrix ModelViewProjectionMatrix;
}

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

	output.Position = mul(ModelViewProjectionMatrix, float4(Position, 1));
	output.Normal_WS = mul((float3x3)InverseTransposeWorldMatrix, Normal);
	output.Color = Color;
	output.UV = UV;
	output.Pos_WS = mul(WorldMatrix, float4(Position, 1));

	return output;
}

// Pixel shader
#include "lighting.hlsl"

cbuffer PixelConstants : register(b1)
{
	float3 LightDirection;
	float Padding;
	float4 CameraPosition;
}

Texture2D		diffuseTexture : register(t0);
SamplerState	defaultSampler : register(s0);

LightingOutput ComputeLighting(float4 Pos_WS, float3 Normal)
{
	float3 CameraDirection = normalize(CameraPosition.xyz - Pos_WS.xyz);

	LightingOutput result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	result.Diffuse = CalculateDiffuse(LightDirection, Normal);
	result.Specular = CalculateSpecular(CameraDirection, LightDirection, Normal);

	return result;
}

float4 PSMain(VtP input) : SV_TARGET
{
	LightingOutput lighting = ComputeLighting(input.Pos_WS, normalize(input.Normal_WS));
	
	float day = LightDirection.y;

	float4 emissive = { 0, 0, 0, 0 };
	float4 ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
	float4 diffuse = lighting.Diffuse;
	float4 specular = lighting.Specular;
	float4 illum = ambient + diffuse + specular;

	return illum * diffuseTexture.Sample(defaultSampler, input.UV);
}
