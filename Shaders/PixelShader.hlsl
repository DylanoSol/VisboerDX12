struct PixelShaderInput
{
	float4 Color    : COLOR;
    float4 Normal : NORMAL; 
};

float4 main( PixelShaderInput IN ) : SV_Target
{
    float3 lightColor = float3(1.0, 1.0, 1.0);
    
    // Ambient light
    float ambientStrength = 1.0;
    float3 ambientLight = ambientStrength * lightColor;
    
    float3 result = (IN.Normal.xyz + 1.0) * 0.5 * ambientLight;
    
    return float4(result, 1.0);
}