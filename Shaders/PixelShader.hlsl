struct PixelShaderInput
{
	float4 Color    : COLOR;
    float4 Normal : NORMAL; 
    float4 LightDir : LIGHTDIRECTION;
    float4 ViewDir : VIEWDIRECTION; 
};

float4 main( PixelShaderInput IN ) : SV_Target
{
    float3 lightColor = float3(1.0, 1.0, 1.0);
    
    float3 norm = normalize(IN.Normal.xyz); 
    
    // Ambient light
    float ambientStrength = 0.1;
    float3 ambientLight = ambientStrength * lightColor;
    
    // Diffuse light
    float diffuseStrength = max(dot(norm, IN.LightDir.xyz), 0.0); 
    float3 diffuseLight = diffuseStrength * lightColor; 
    
    // Specular light
    float3 reflectDir = reflect(-IN.LightDir.xyz, norm.xyz); 
    
    float specularStrength = 0.5; 
    float spec = pow(max(dot(IN.ViewDir.xyz, reflectDir), 0.0), 32); 
    
    float3 specularLight = specularStrength * spec * lightColor; 
    
    float3 result = (norm + 1.0) * 0.5 * (ambientLight + diffuseLight + specularLight);
    
    return float4(result, 1.0);
}