struct LightingOutput
{
	float4 Diffuse;
	float4 Specular;
};

float4 CalculateDiffuse(float3 LightDirection, float3 Normal)
{
	float diffuseDot = max(0, dot(Normal, LightDirection));
	return diffuseDot;
}

float4 CalculateSpecular(float3 CameraDirection, float3 LightDirection, float3 Normal)
{
	// Phong lighting.
	float3 R = normalize(reflect(-LightDirection, Normal));
	float RdotV = max(0, dot(R, CameraDirection));

	// Blinn-Phong lighting
	float3 H = normalize( LightDirection + CameraDirection );
	float NdotH = max( 0, dot( Normal, H ) );

	return pow( RdotV, 10 );
}