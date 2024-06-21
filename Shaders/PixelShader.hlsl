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
    float4 InPosition : INCOMINGPOSITION; 
};

ConstantBuffer<JustModelMatrix> ModelCB : register(b1);
ConstantBuffer<LightPos> LightPosCB : register(b2);
ConstantBuffer<ViewPos> ViewPosCB : register(b3);


float4 main( PixelShaderInput IN ) : SV_Target
{
    
    // World space calculation in pixel shader 
    float3 worldPosition = mul(ModelCB.MODEL, float4(IN.InPosition.xyz, 1.0)).xyz;
    
    // Normal calculation in pixel shader. Cast to float3x3 to not include translation component
    float3 norm = normalize(mul((float3x3)ModelCB.MODEL, IN.Normal.xyz));
    
    // Lighting calculations
    float3 lightDir = normalize(LightPosCB.POS - worldPosition);
    
    float3 viewDir = normalize(ViewPosCB.POS - worldPosition);
    
    float3 lightColor = float3(1.0, 1.0, 1.0);
    
    // Ambient light
    float ambientStrength = 0.1;
    float3 ambientLight = ambientStrength * lightColor;
    
    // Diffuse light
    float diffuseStrength = max(dot(norm, lightDir.xyz), 0.0); 
    float3 diffuseLight = diffuseStrength * lightColor; 
    
    // Specular light
    float3 reflectDir = reflect(-lightDir.xyz, norm.xyz); 
    
    float specularStrength = 0.2; 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); 
    
    float3 specularLight = specularStrength * spec * lightColor; 
    
    float3 result = (norm + 1.0) * 0.5 * (ambientLight + diffuseLight + specularLight);
    
    return float4(result, 1.0);
}