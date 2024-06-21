struct JustModelMatrix
{
    matrix MODEL;
};

struct LightPos
{
    float3 POS; 
};

struct ViewPos
{
    float3 POS; 
};

struct PixelShaderInput
{
	float4 Color    : COLOR;
    float4 Normal : NORMAL; 
    float4 WorldPos : WORLDPOSITION; 
};

ConstantBuffer<LightPos> LightPosCB : register(b2);
ConstantBuffer<ViewPos> ViewPosCB : register(b3);


float4 main( PixelShaderInput IN ) : SV_Target
{
    // Lighting calculations
    float3 lightDir = float4(normalize(LightPosCB.POS - IN.WorldPos.xyz), 1.0);
    
    float3 viewDir = float4(normalize(ViewPosCB.POS - IN.WorldPos.xyz), 1.0);
    
    float3 lightColor = float3(1.0, 1.0, 1.0);
    
    float3 norm = normalize(IN.Normal.xyz); 
    
    // Ambient light
    float ambientStrength = 0.1;
    float3 ambientLight = ambientStrength * lightColor;
    
    // Diffuse light
    float diffuseStrength = max(dot(norm, lightDir.xyz), 0.0); 
    float3 diffuseLight = diffuseStrength * lightColor; 
    
    // Specular light
    float3 reflectDir = reflect(-lightDir.xyz, norm.xyz); 
    
    float specularStrength = 0.5; 
    float spec = pow(max(dot(viewDir.xyz, reflectDir), 0.0), 32); 
    
    float3 specularLight = specularStrength * spec * lightColor; 
    
    float3 result = (norm + 1.0) * 0.5 * (ambientLight + diffuseLight + specularLight);
    
    return float4(result, 1.0);
}