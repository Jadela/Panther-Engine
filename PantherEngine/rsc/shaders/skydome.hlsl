cbuffer AppCB : register(b0)
{
	float4 m_ScreenResolution;
};

cbuffer FrameCB : register(b1)
{
	float4 m_Light0Direction;
	float4 m_CameraPosition;
	float m_Time;
};

cbuffer ObjectCB : register(b2)
{
	matrix m_MVP;
	matrix m_M;
	matrix m_IT_M;
};

struct VtP
{
	float4 Position	: SV_POSITION;
	float2 UV		: TEXCOORD;
	float3 Sun_WS	: SUN_WS;
	float4 Sun_SS	: SUN_SS;
	float4 Pos_SS	: POSITION;
	float4 Moon_SS	: MOON_SS;
};

VtP VSMain(float3 Position : POSITION, float2 UV : TEXCOORD)
{
	VtP output;

	output.Position = mul(m_MVP, float4(Position, 1));
	output.UV = UV;
	output.Sun_WS = m_Light0Direction.xyz;
    output.Sun_SS = mul(m_MVP, float4(m_Light0Direction.xyz, 1));
    output.Moon_SS = mul(m_MVP, float4(-m_Light0Direction.xyz, 1));
	output.Pos_SS = output.Position;

	return output;
}

// Pixel shader
cbuffer PixelCB : register(b4)
{
	float2 ScreenDimensions;
}

Texture2D		dayTexture : register(t0);
Texture2D		nightTexture : register(t1);
Texture2D		sunTexture : register(t2);
Texture2D		moonTexture : register(t3);
SamplerState	clampedSampler : register(s0);

float4 PSMain(VtP input) : SV_TARGET
{
	float AspectRatio = ScreenDimensions.y / ScreenDimensions.x;

	float4 SunC = float4(0, 0, 0, 0);
	float4 MoonC = float4(0, 0, 0, 0);

	float2 SS_Pos = input.Pos_SS.xy / input.Pos_SS.w;
	float2 SS_Sun = input.Sun_SS.xy / input.Sun_SS.w;
	float2 SS_Moon = input.Moon_SS.xy / input.Moon_SS.w;
	float2 UV_Sun = SS_Sun - SS_Pos;
	UV_Sun.y *= AspectRatio;
	UV_Sun += float2(0.5, 0.5);
	float2 UV_Moon = SS_Moon - SS_Pos;
	UV_Moon.y *= AspectRatio;
	UV_Moon += float2(0.5, 0.5);

	if (input.Sun_SS.z > 0.0)
		SunC = sunTexture.Sample(clampedSampler, UV_Sun);
	else
	{
		UV_Moon *= 10;
		MoonC = moonTexture.Sample(clampedSampler, UV_Moon);
	}

	float SunFullBrightAngle = 40.0;
	float SunFullDarkAngle = -20.0;

	float SunAngle = input.Sun_WS.y * 90;
	float day = clamp(SunAngle / SunFullBrightAngle, 0.0f, 1.0f);
	float night = clamp(SunAngle / SunFullDarkAngle, 0.0f, 1.0f) * 0.8f;
	float4 skyColor = (1.0f - night) * (day * dayTexture.Sample(clampedSampler, input.UV.yx) + (1.0f - day) * nightTexture.Sample(clampedSampler, input.UV.yx));
	
	return skyColor + SunC + MoonC;
}