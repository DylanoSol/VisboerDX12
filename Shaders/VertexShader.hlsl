struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexPosColor
{
    float3 Position : POSITION;
    float3 Color    : COLOR;
};

struct VertexShaderOutput
{
	float4 Color    : COLOR;
    float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
    
    float3 lightColor = float3(1.0, 1.0, 1.0); 
    
    // Ambient light
    float ambientStrength = 0.3; 
    float3 ambientLight = ambientStrength * lightColor; 
    
    float3 result = IN.Color * ambientLight; 
    
    OUT.Color = float4(result, 1.0);

    return OUT;
}